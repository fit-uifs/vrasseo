#pragma once
#ifndef FULLFRAMEBLENDER_H
#define FULLFRAMEBLENDER_H

#include "FrameBlender.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace FrameFilters
{


class FullFrameBlender : public FrameBlender
{
private:
	virtual void blend(Frame & background, Frame & foreground, FrameMask & mask)
	{
		foreground.copyTo(background);
	}

public:
	FullFrameBlender() : FrameBlender()
	{
	}

	FullFrameBlender(FrameFilterPtr base, Frame & foreground, FrameMask & mask) : FrameBlender(base, foreground, mask)
	{
	}

	FullFrameBlender(Frame & base, Frame & foreground, FrameMask & mask) : FrameBlender(base, foreground, mask)
	{
	}

	virtual ~FullFrameBlender()
	{
	}
};


typedef cv::Ptr<FullFrameBlender> FullFrameBlenderPtr;


}}}

#endif // FULLFRAMEBLENDER_H
