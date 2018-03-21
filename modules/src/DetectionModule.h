#pragma once
#include "ProgressUpdater.h"
#include <vtapi/plugins/module_interface.h>
#include <atomic>
#include <memory>

namespace Modules {


class DetectionModule : public vtapi::IModuleInterface
{
public:
    DetectionModule();
    ~DetectionModule();

    void initialize(vtapi::VTApi & vtapi) override;
    void uninitialize() noexcept override;
    void process(vtapi::Process & process) override;
    void stop() noexcept override;

private:
    std::shared_ptr<ProgressUpdater> _progress_updater;
    std::atomic_bool _stop;
};


}
