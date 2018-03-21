#pragma once

#include <memory>
#include "IEventDetection.h"
#include "Actdet.h"

namespace Modules {
namespace Actdet {


class ActdetEventDetectionImpl : public IEventDetection
{
public:
    void initialize(const vtapi::TaskParams & params) override;
    void uninitialize() noexcept override;
    void processVideo(const vtapi::Video & video,
                      vtapi::Interval & input,
                      vtapi::IntervalOutput & output,
                      ::Modules::IVideoProgressUpdater & progress) override;
    void stop() noexcept override;

private:
    ActDetEdParams _params;
};


}
}
