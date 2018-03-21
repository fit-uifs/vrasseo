#pragma once
#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include "Exceptions.h"

namespace SummarizationFramework
{


#define FRAMESPAN_EMPTY -1

typedef cv::Mat Frame;
typedef cv::Ptr<Frame> FramePtr;

typedef cv::Mat FrameMask;
typedef cv::Ptr<FrameMask> FrameMaskPtr;

typedef int FramePosition;
typedef cv::Size FrameSize;

struct FrameSpan {
	FramePosition From;
	FramePosition To;
	
	FrameSpan(FramePosition from = FRAMESPAN_EMPTY, FramePosition to = FRAMESPAN_EMPTY)
	{
		From = from;
		To = to;
	}

	int GetLength() const
	{
		return To - From + 1;
	}

	FrameSpan Intersect(FrameSpan & other) const
	{
		if(!IsIntersecting(other))
		{
			return FrameSpan();
		}
	
		return FrameSpan(MAX(From, other.From), MIN(To, other.To));
	}

	bool IsEmpty() const
	{
		return From == FRAMESPAN_EMPTY || To == FRAMESPAN_EMPTY;
	}

	bool IsIntersecting(FrameSpan & other) const
	{
		return From <= other.To && To >= other.From;
	}
};


}

#endif // COMMONTYPES_H
