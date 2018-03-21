#include "DetectionModule.h"
#include "ProgressUpdater.h"
#include "interfaces/IDetectionModule.h"
#include <Poco/ClassLibrary.h>
#include <memory>

namespace Modules {


DetectionModule::DetectionModule()
    : _stop(false)
{
}

DetectionModule::~DetectionModule()
{

}

void DetectionModule::initialize(vtapi::VTApi & vtapi)
{
    _progress_updater = std::make_shared<ProgressUpdater>(vtapi);
    _stop = false;
}

void DetectionModule::uninitialize() noexcept
{
    _progress_updater.reset();
}

void DetectionModule::process(vtapi::Process & process)
{
    using std::shared_ptr;

    try
    {
        // current dataset object
        shared_ptr<vtapi::Dataset> dataset(process.getParentDataset());
        if (!dataset) throw vtapi::RuntimeModuleException("Failed to get process's dataset");

        // current task object
        shared_ptr<vtapi::Task> task(process.getParentTask());
        if (!task) throw vtapi::RuntimeModuleException("Failed to get process's task");

        // load videos assigned to this process
        shared_ptr<vtapi::Video> video(process.loadAssignedVideos());

        // call module's initialization
        IDetectionModule::instance().initialize(task->getParams());

        // initialize progress reporter and update process state to running
        _progress_updater->run(process);
        _progress_updater->setRunningState();

        // processing loop
        while (video->next()) {
            // data outputting object
            shared_ptr<vtapi::IntervalOutput> output(task->createIntervalOutput(video->getName()));

            // update process state to current video
            std::string vidname = video->getName();
            _progress_updater->setProcessedItem(vidname);

            // lock video for processing by this process and process it
            shared_ptr<vtapi::TaskProgress> tprog (process.acquireSequenceLock(vidname));
            if (tprog) {
                try
                {
                    IDetectionModule::instance().processVideo(*video, *output, *_progress_updater);
                    output->commit();   // commit all output for this video
                }
                catch (vtapi::Exception &e)
                {
                    // error => discard all previously outputted data for this video
                    output->discard(false);
                    throw;
                }

                // mark video as processed by this process
                tprog->updateIsDone(true);
            }

            if (_stop) throw vtapi::UserAbortModuleException();
        }

        // update state - all videos are processed
        _progress_updater->setProcessedItem(std::string());

        // call module's uninitialization
        IDetectionModule::instance().uninitialize();
    }
    catch (vtapi::Exception &e)
    {
        // known error => update state
        IDetectionModule::instance().uninitialize();
        _progress_updater->setErrorState(e.message());
        throw;
    }
    catch (std::exception &e)
    {
        // unknown error => try setting state and rethrow
        IDetectionModule::instance().uninitialize();
        std::string errmsg = std::string("FATAL ERROR: ") + e.what();
        _progress_updater->setErrorState(errmsg);
        throw vtapi::RuntimeModuleException(errmsg);
    }

    _progress_updater->setFinishedState();
}

void DetectionModule::stop() noexcept
{
    _stop = true;
    IDetectionModule::instance().stop();
}


}


POCO_BEGIN_MANIFEST(vtapi::IModuleInterface)
    POCO_EXPORT_CLASS(Modules::DetectionModule)
POCO_END_MANIFEST
