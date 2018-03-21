/*
 * TODO
 * by: Lukas Klicnar (iklicnar[at]fit.vutbr.cz)
 */

#include "VtOutputHandler.h"

namespace Modules
{
namespace Activity
{


void VtOutputHandler::takeTube(SummarizationFramework::Model::TubePtr tube)
{
	SummarizationFramework::Model::TubeFrameCollectionPtr frames = tube->GetFrames();

	if(frames->size() < 2)
	{
		return;
	}

	SummarizationFramework::FrameMask mask = (*frames)[0]->GetData()->GetMask();	
	SummarizationFramework::FrameSpan framespan = tube->GetFrameSpan();

	double min_x1 = mask.cols-1;
	double min_y1 = mask.rows-1;
	double max_x2 = 0;
	double max_y2 = 0;

	// get bounding box for all frames
	SummarizationFramework::Model::TubeFrameCollection::iterator iterator;
	for(iterator = frames->begin(); iterator != frames->end(); iterator++)
	{
		cv::Rect bbox = (*iterator)->GetBoundingBox();
		min_x1 = MIN(min_x1, bbox.x);
		min_y1 = MIN(min_y1, bbox.y);
		max_x2 = MAX(max_x2, bbox.x+bbox.width);
		max_y2 = MAX(max_y2, bbox.y+bbox.height);
	}

	// normalize to 0..1
	min_x1 = (double) min_x1/mask.cols;
	min_y1 = (double) min_y1/mask.rows;
	max_x2 = (double) max_x2/mask.cols;
	max_y2 = (double) max_y2/mask.rows;

    double width = std::abs(min_x1-max_x2);
    double height = std::abs(min_y1-max_y2);

	// check for minimal size
	if(width < params.minWidth || width > params.maxWidth || height < params.minHeight || height > params.maxHeight)
	{
		return;
	}

	// event passed the test => generate group_id
	int currentGroupId = nextGroupId++;

	// create and store root event
	//output->add(tube->GetFileName(), framespan.From, framespan.To, "");

	vtapi::IntervalEvent::Box region;
	region.high.x = max_x2;
	region.high.y = max_y2;
	region.low.x = min_x1;
	region.low.y = min_y1;

	vtapi::IntervalEvent event;
	event.group_id = currentGroupId;
        event.class_id = 1;
        event.is_root = true;
        event.region = region;
        event.score = 1;

        output->newInterval(framespan.From, framespan.To);
        output->setIntervalEvent(def_key_event, event);

	// create and store non-root events (trajectory)
	int counter = 0;
	for(iterator = frames->begin(); iterator != frames->end(); iterator++)
	{
		if(counter%params.keyframeFreq == 0 || counter == 0 || counter == frames->size()-1)
		{
			cv::Rect bbox = (*iterator)->GetBoundingBox();
			
			// normalize to 0..1
			vtapi::IntervalEvent::Box region;
			region.high.x =(double) (bbox.x + bbox.width)/mask.cols;
			region.high.y = (double) (bbox.y + bbox.height)/mask.rows;
			region.low.x = (double) bbox.x/mask.cols;
			region.low.y = (double) bbox.y/mask.rows;
			
			vtapi::IntervalEvent event;
			event.group_id = currentGroupId;
        		event.class_id = 1;
        		event.is_root = false;
        		event.region = region;
        		event.score = 1;
			
			//output->addIntervalEvent(def_key_event_offline, event);
			//output->addExecute();
			SummarizationFramework::FramePosition framePosition = (*iterator)->GetFramePosition();
                        output->newInterval(framePosition, framePosition);
                        output->setIntervalEvent(def_key_event, event);

                        if(++commit == 500)
                        {
                                output->commit();
                                commit = 0;
                        }
		}

		counter++;
	}
}

VtOutputHandler::VtOutputHandler(vtapi::IntervalOutput *output, DetectionParams params) : SummarizationFramework::ImageProcessing::TubesExtraction::IOutputHandler()
{
	this->output = output;
	this->params = params;
	this->nextGroupId = 1;
	this->commit = 0;
}

VtOutputHandler::~VtOutputHandler()
{
}


}
}

