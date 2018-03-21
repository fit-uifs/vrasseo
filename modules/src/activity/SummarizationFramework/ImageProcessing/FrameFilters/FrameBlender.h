#pragma once
#ifndef FRAMEBLENDER_H
#define FRAMEBLENDER_H

#include "FrameFilter.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace FrameFilters
{


class FrameBlender : public FrameFilter
{
private:
	Frame foreground;
	FrameMask mask;

	virtual void apply(Frame & frame)
	{
		blend(frame, foreground, mask);
	}

protected:
	virtual void blend(Frame & background, Frame & foreground, FrameMask & mask)
	{
	}

public:
	FrameBlender() : FrameFilter()
	{
		setForeground(Frame());
		setMask(FrameMask());
	}
	
	FrameBlender(FrameFilterPtr base, Frame & foreground, FrameMask & mask) : FrameFilter(base)
	{
		setForeground(foreground);
		setMask(mask);
	}

	FrameBlender(Frame & base, Frame & foreground, FrameMask & mask) : FrameFilter(base)
	{
		setForeground(foreground);
		setMask(mask);
	}

	virtual ~FrameBlender()
	{
	}

	void setForeground(Frame foreground)
	{
		this->foreground = foreground;
	}

	void setMask(FrameMask mask)
	{
		this->mask = mask;
	}
};


typedef cv::Ptr<FrameBlender> FrameBlenderPtr;


}}}

#endif // FRAMEBLENDER_H
