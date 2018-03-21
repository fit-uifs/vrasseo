/*
 * Adapter of VTAPI interface to SummarizationFramework.
 * by: Lukas Klicnar (iklicnar[at]fit.vutbr.cz)
 */

#pragma once

#include "../ProgressUpdater.h"
#include "SummarizationFramework/Model/IVideoSource.h"
#include <opencv2/opencv.hpp>
#include <vtapi/vtapi.h>

namespace Modules
{
namespace Activity
{


class VtVideoAdapter : public SummarizationFramework::Model::IVideoSource
{
public:
	class EventsHandler
	{
	public:
		virtual void ProgressChanged(VtVideoAdapter *video, int progress) = 0;
	};

	typedef cv::Ptr<EventsHandler> EventsHandlerPtr;

	VtVideoAdapter(const vtapi::Video& video);
	virtual ~VtVideoAdapter();
	virtual void Reset();
	virtual bool Next();
	virtual SummarizationFramework::Frame GetFrame();
	virtual int GetFrameNumber();
	virtual int GetLength();
	void SetEventsHandler(EventsHandlerPtr handler);

protected:
	virtual bool seek(int frame);
	void triggerProgressChanged(int progress);

private:
	cv::VideoCapture _capture;
	cv::Mat _frame;
	int _length;
	int _framesPerPercent;
	int _nextPercentFramesLeft;
	int _percentage;

	EventsHandlerPtr _eventsHandler;
};

typedef cv::Ptr<VtVideoAdapter> VtVideoAdapterPtr;


class VtProgressEventsHandler : public VtVideoAdapter::EventsHandler
{
private:
    IProgressUpdater *progressUpdater;

public:
    VtProgressEventsHandler(IProgressUpdater *progressUpdater);
	virtual void ProgressChanged(VtVideoAdapter *video, int progress);
};

typedef cv::Ptr<VtProgressEventsHandler> VtProgressEventsHandlerPtr;


}}

