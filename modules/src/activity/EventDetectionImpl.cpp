#include "EventDetectionImpl.h"

namespace Modules {


IEventDetection &IEventDetection::instance()
{
    static Actdet::ActdetEventDetectionImpl instance;
    return instance;
}


namespace Actdet {


void ActdetEventDetectionImpl::initialize(const vtapi::TaskParams & params)
{
	std::cout << "ED init" << std::endl;
    if(!params.getDouble(def_param_min_width, _params.minWidth) ||
        !params.getDouble(def_param_min_height, _params.minHeight) ||
        !params.getDouble(def_param_max_width, _params.maxWidth) ||
        !params.getDouble(def_param_max_height, _params.maxHeight))
    {
        throw ActdetModuleException("Missing some parameter(s)");
    }

    if(_params.minWidth < 0 ||
       _params.minHeight < 0 ||
       _params.maxWidth > 1 ||
       _params.maxHeight > 1 ||
       _params.maxWidth <= _params.minWidth ||
       _params.maxHeight <= _params.minHeight)
    {
       throw ActdetModuleException("Invalid value of some parameter(s)");
    }

    std::cout << "ED init done: " << _params.minWidth << std::endl;
}

void ActdetEventDetectionImpl::uninitialize() noexcept
{
}

void ActdetEventDetectionImpl::processVideo(const vtapi::Video &video,
                                            vtapi::Interval &input,
                                            vtapi::IntervalOutput &output,
                                            Modules::IVideoProgressUpdater &progress)
{
    std::cout << "processVideo start" << std::endl;
    int selectedGroupId = -1;

    while(input.next())
    {
        vtapi::IntervalEvent event = input.getIntervalEvent(def_key_event_offline);

        double width = (event.region.high.x - event.region.low.x);
        double height = (event.region.high.y - event.region.low.y);
	if (width < 0) width = width * (0 - 1);
	if (height < 0) height = height * (0 - 1);
	std::cout << event.region.low.x << " - " << event.region.high.x << " = " << width << " " << height << " " << std::endl;
        if(event.is_root &&
          width >= _params.minWidth &&
          width <= _params.maxWidth &&
          height >= _params.minHeight &&
          height <= _params.maxHeight)
        {
	   std::cout << "selected" << std::endl;
            selectedGroupId = event.group_id;
        }

        if(selectedGroupId == event.group_id)
        {
            output.newInterval(input.getStartTime(), input.getEndTime());
            output.setIntervalEvent(def_key_event, event);
        }
    }
   std::cout << "processVideo end" << std::endl;

}

void ActdetEventDetectionImpl::stop() noexcept
{
    // TODO: stop
}

}
}
