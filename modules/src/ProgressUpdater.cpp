#include "ProgressUpdater.h"

namespace Modules {


const int ProgressUpdater::_update_period_ms;


ProgressUpdater::ProgressUpdater(vtapi::VTApi & vtapi)
    : _vtapi(vtapi), _state_changed(false), _stop_flag(false)
{
}

ProgressUpdater::~ProgressUpdater()
{
    if (_thread.joinable()) {
        _stop_flag = true;
        _thread.join();
    }
}

void ProgressUpdater::update(double progress) noexcept
{
    std::lock_guard<std::mutex> lk(_state_mutex);
    _state.progress = progress;
    _state_changed = true;
}

void ProgressUpdater::update(int progress) noexcept
{
    std::lock_guard<std::mutex> lk(_state_mutex);
    _state.progress = static_cast<double>(progress) / 100;
    _state_changed = true;
}

void ProgressUpdater::run(const vtapi::Process &process)
{
    _thread = std::thread(&ProgressUpdater::updaterLoop, this, process.getId());
}

void ProgressUpdater::setProcessedItem(const std::string &processed_item)
{
    std::lock_guard<std::mutex> lk(_state_mutex);
    _state.current_item = processed_item;
    _state.progress = processed_item.empty() ? 1.0 : 0.0;
    _state_changed = true;
}

void ProgressUpdater::setRunningState()
{
    {
        std::lock_guard<std::mutex> lk(_state_mutex);
        _state.status = vtapi::ProcessState::STATUS_RUNNING;
        _state_changed = true;
    }
    _cv_update_now.notify_one();
}

void ProgressUpdater::setFinishedState()
{
    {
        std::lock_guard<std::mutex> lk(_state_mutex);
        _state.status = vtapi::ProcessState::STATUS_FINISHED;
        _state_changed = true;
    }
    _cv_update_now.notify_one();
}

void ProgressUpdater::setErrorState(const std::string &error_message)
{
    {
        std::lock_guard<std::mutex> lk(_state_mutex);
        _state.status = vtapi::ProcessState::STATUS_ERROR;
        _state.last_error = error_message;
        _state_changed = true;
    }
    _cv_update_now.notify_one();
}

void ProgressUpdater::updaterLoop(int process_id)
{
    // TODO: try catch init errors

    vtapi::VTApi vtapi(_vtapi);
    std::shared_ptr<vtapi::Process> pprocess(vtapi.loadProcesses(process_id));
    pprocess->next();

    std::unique_lock<std::mutex> lk(_state_mutex);
    do {
        if (!_state_changed)
            _cv_update_now.wait_for(lk, std::chrono::milliseconds(_update_period_ms));

        if (_state_changed) {
            vtapi::ProcessState state = _state;
            _state_changed = false;
            lk.unlock();
            try
            {
                if (state.progress < 0.0)
                    state.progress = 0.0;
                else if (state.progress > 1.0)
                    state.progress = 1.0;

                pprocess->updateState(state);
                pprocess->updateExecute();
            }
            catch (...) {}
            lk.lock();
        }
    } while (!_stop_flag);
}


}
