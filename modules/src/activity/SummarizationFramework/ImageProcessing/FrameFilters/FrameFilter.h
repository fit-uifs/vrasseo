#pragma once
#ifndef FRAMEFILTER_H
#define FRAMEFILTER_H

#include "../../CommonTypes.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace FrameFilters
{


class FrameFilter;
typedef cv::Ptr<FrameFilter> FrameFilterPtr;


class FrameFilter
{
private:
	FrameFilterPtr previous;
	Frame baseFrame;

protected:
	virtual void apply(Frame & frame)
	{
	}

public:
	FrameFilter(FrameFilterPtr base)
	{
		SetBase(base);
	}

	FrameFilter(Frame base = Frame())
	{
		SetBase(base);
	}

	virtual ~FrameFilter()
	{
	}

	void SetBase(FrameFilterPtr base)
	{
		previous = base;
		baseFrame = Frame();
	}

	void SetBase(Frame & base)
	{
		previous = NULL;
		baseFrame = base;
	}

	Frame GetFrame()
	{
		Frame frame;
		
		if(previous == NULL)
		{
			frame = baseFrame;
		}
		else
		{
			frame = previous->GetFrame();
		}

		if(!frame.empty())
		{
			apply(frame);
		}

		return frame;
	}

	void SetFrame(Frame & frame)
	{
		FrameFilter *current = this;
		while(current->previous != NULL)
		{
			current = current->previous;
		}

		current->SetBase(frame);
	}
};


}}}

#endif // FRAMEFILTER_H
