#pragma once

#include <sstream>
#include <memory>
#include "ActivityDefs.h"
#include "IDetectionModule.h"
#include "VtVideoAdapter.h"
#include "VtOutputHandler.h"
#include "SummarizationFramework/ImageProcessing/TubesExtraction/MogTubesExtractor.h"

namespace Modules {
namespace Activity {


class ActivityDetectionModule: public IDetectionModule
{
public:
    void initialize(const vtapi::TaskParams &params) override;
    void uninitialize() noexcept override;
    void processVideo(const vtapi::Video & video,
                      vtapi::IntervalOutput &output,
                      ::Modules::IProgressUpdater & progress) override;
    void stop() noexcept override;

private:
    DetectionParams _params;
    SummarizationFramework::ImageProcessing::TubesExtraction::MogTubesExtractorPtr _extractor;
};


}
}
