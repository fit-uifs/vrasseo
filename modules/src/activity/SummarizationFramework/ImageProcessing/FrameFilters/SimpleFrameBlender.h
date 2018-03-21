#pragma once
#ifndef SIMPLEFRAMEBLENDER_H
#define SIMPLEFRAMEBLENDER_H

#include "FrameBlender.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace FrameFilters
{


class SimpleFrameBlender : public FrameBlender
{
private:
	virtual void blend(Frame & background, Frame & foreground, FrameMask & mask)
	{
		foreground.copyTo(background, mask);
	}

public:
	SimpleFrameBlender() : FrameBlender()
	{
	}

	SimpleFrameBlender(FrameFilterPtr base, Frame & foreground, FrameMask & mask) : FrameBlender(base, foreground, mask)
	{
	}

	SimpleFrameBlender(Frame & base, Frame & foreground, FrameMask & mask) : FrameBlender(base, foreground, mask)
	{
	}

	virtual ~SimpleFrameBlender()
	{
	}
};


typedef cv::Ptr<SimpleFrameBlender> SimpleFrameBlenderPtr;


}}}

#endif // SIMPLEFRAMEBLENDER_H
