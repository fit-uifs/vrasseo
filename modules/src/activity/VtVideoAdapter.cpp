/*
 * Adapter of VTAPI interface to SummarizationFramework.
 * by: Lukas Klicnar (iklicnar[at]fit.vutbr.cz)
 */

#include "ActivityDefs.h"
#include "VtVideoAdapter.h"

using namespace std;
using namespace vtapi;

namespace Modules {
namespace Activity {


VtVideoAdapter::VtVideoAdapter(const Video& video)
    : SummarizationFramework::Model::IVideoSource()
{
	_capture = video.openVideo(); // throws exception
	_eventsHandler.release();
	
	_length = static_cast<int>(_capture.get(CV_CAP_PROP_FRAME_COUNT));
	_framesPerPercent = std::ceil(_length/100);
	_nextPercentFramesLeft = _framesPerPercent;
	_percentage = 0;
}

VtVideoAdapter::~VtVideoAdapter()
{
}

bool VtVideoAdapter::seek(int frame)
{
	bool seekOk = _capture.set(CV_CAP_PROP_POS_FRAMES, frame);

	if(seekOk)
	{
		_percentage = std::floor((double)frame/_length*100);
		triggerProgressChanged(_percentage);
	}
	return seekOk;
}

void VtVideoAdapter::triggerProgressChanged(int progress)
{
	if(_eventsHandler != NULL)
	{
		_eventsHandler->ProgressChanged(this, std::min(std::max(progress, 0), 100));
	}
}

void VtVideoAdapter::Reset()
{
	_capture.set(CV_CAP_PROP_POS_FRAMES, 0);
	triggerProgressChanged(_percentage = 0);
}

void VtVideoAdapter::SetEventsHandler(EventsHandlerPtr handler)
{
	_eventsHandler = handler;
}

bool VtVideoAdapter::Next()
{
 	_capture >> _frame;

	if(--_nextPercentFramesLeft <= 0)
	{
		_nextPercentFramesLeft = _framesPerPercent;
		triggerProgressChanged(++_percentage);
	}

	return !_frame.empty();
}

SummarizationFramework::Frame VtVideoAdapter::GetFrame()
{
	return _frame;
}

int VtVideoAdapter::GetFrameNumber()
{
	return static_cast<int>(_capture.get(CV_CAP_PROP_POS_FRAMES));
}

int VtVideoAdapter::GetLength()
{
	return _length;
}


VtProgressEventsHandler::VtProgressEventsHandler(IProgressUpdater *progressUpdater)
{
	this->progressUpdater = progressUpdater;
}

void VtProgressEventsHandler::ProgressChanged(VtVideoAdapter *video, int progress)
{
	this->progressUpdater->update(progress);
}

}
}

