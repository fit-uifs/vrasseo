#pragma once
#ifndef SEAMLESSFRAMEBLENDER_H
#define SEAMLESSFRAMEBLENDER_H

#include "../seamless_cloning.hpp"
#include "FrameBlender.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace FrameFilters
{


class SeamlessFrameBlender : public FrameBlender
{
private:
	virtual void blend(Frame & background, Frame & foreground, FrameMask & mask)
	{
		// TODO neni seamless, ze...
		//foreground.copyTo(background, mask);
		
		Seamless::seamlessClone(foreground, background, mask, cv::Point(0,0), background, 1);
	}

public:
	SeamlessFrameBlender() : FrameBlender()
	{
	}

	SeamlessFrameBlender(FrameFilterPtr base, Frame & foreground, FrameMask & mask) : FrameBlender(base, foreground, mask)
	{
	}

	SeamlessFrameBlender(Frame & base, Frame & foreground, FrameMask & mask) : FrameBlender(base, foreground, mask)
	{
	}

	virtual ~SeamlessFrameBlender()
	{
	}
};


typedef cv::Ptr<SeamlessFrameBlender> SeamlessFrameBlenderPtr;


}}}

#endif // SEAMLESSFRAMEBLENDER_H
