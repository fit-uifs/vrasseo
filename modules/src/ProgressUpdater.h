#pragma once

#include "interfaces/IProgressUpdater.h"
#include <vtapi/vtapi.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

namespace Modules {


class ProgressUpdater : public IProgressUpdater
{
public:
    ProgressUpdater(vtapi::VTApi &process);
    ~ProgressUpdater();

    void update(double progress) noexcept override;
    void update(int progress) noexcept override;

    void run(const vtapi::Process &process);
    void setProcessedItem(const std::string &processed_item);
    void setRunningState();
    void setFinishedState();
    void setErrorState(const std::string &error_message);

private:
    static const int _update_period_ms = 1000;

    vtapi::VTApi &_vtapi;
    vtapi::ProcessState _state;
    bool _state_changed;
    std::mutex _state_mutex;

    std::thread _thread;
    std::atomic_bool _stop_flag;
    std::condition_variable _cv_update_now;

    void updaterLoop(int process_id);

    ProgressUpdater() = delete;
    ProgressUpdater(const ProgressUpdater &) = delete;
    ProgressUpdater & operator=(const ProgressUpdater &) = delete;
};


}
