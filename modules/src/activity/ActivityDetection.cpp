#include "ActivityDetection.h"
#include <chrono>
#include <thread>

using namespace SummarizationFramework::Model;
using namespace SummarizationFramework::ImageProcessing::TubesExtraction;

namespace Modules {


IDetectionModule &IDetectionModule::instance()
{
    static Activity::ActivityDetectionModule instance;
    return instance;
}


namespace Activity {


void ActivityDetectionModule::initialize(const vtapi::TaskParams &params)
{
    if (!params.getInt(def_param_keyfreq, _params.keyframeFreq) ||
        !params.getDouble(def_param_min_width, _params.minWidth) ||
        !params.getDouble(def_param_min_height, _params.minHeight) ||
        !params.getDouble(def_param_max_width, _params.maxWidth) ||
        !params.getDouble(def_param_max_height, _params.maxHeight))
    {
        throw ActivityModuleException("Missing some parameter(s)");
    }

    if(_params.keyframeFreq < 1 ||
       _params.minWidth < 0 ||
       _params.minHeight < 0 ||
       _params.maxWidth > 1 ||
       _params.maxHeight > 1 ||
       _params.maxWidth <= _params.minWidth ||
       _params.maxHeight <= _params.minHeight)
    {
       throw ActivityModuleException("Invalid value of some parameter(s)");
    }
}

void ActivityDetectionModule::uninitialize() noexcept
{
    _extractor.release();
}

void ActivityDetectionModule::processVideo(const vtapi::Video &video,
                                             vtapi::IntervalOutput &output,
                                             Modules::IProgressUpdater &progress)
{
//    std::this_thread::sleep_for(std::chrono::seconds(10));

    try
    {
        // create tubes extractor
        VtOutputHandlerPtr outputHandler = new VtOutputHandler(&output, _params);		
        _extractor = new MogTubesExtractor(outputHandler);

        // create video source
        VtVideoAdapterPtr sequence = new VtVideoAdapter(video);
        sequence->SetEventsHandler(new VtProgressEventsHandler(&progress));

        // extract tubes
        _extractor->ProcessVideoSource(sequence, video.getName());
    }
    catch(...)
    {   // TODO slo by i lip :)
        throw ActivityModuleException("Video processing failed.");
    }
}

void ActivityDetectionModule::stop() noexcept
{
    if(_extractor != NULL)
    {
        _extractor->RequestStop();
    }
}


}
}
