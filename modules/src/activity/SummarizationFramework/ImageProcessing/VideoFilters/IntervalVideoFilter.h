#pragma once
#ifndef INTERVALVIDEOFILTER_H
#define INTERVALVIDEOFILTER_H

#include <algorithm>
#include <sstream>
#include "../../CommonTypes.h"
#include "../../Model/VideoSequence.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace VideoFilters
{


class IntervalVideoFilter : public Model::IVideoSource
{
private:
	Model::IVideoSourcePtr source;
	FrameSpan interval;
	int currentFrameNumber;

protected:
	virtual bool seek(int frame)
	{
		CV_Assert(frame >= 1 && frame <= GetLength());

		if(frame != currentFrameNumber)
		{
			int originalFrameNumberToSeek = interval.From + frame - 1;

			if(!source->Seek(originalFrameNumberToSeek))
			{
				return false;
			}

			currentFrameNumber = frame;
		}

		return true;
	}

public:
	IntervalVideoFilter(Model::IVideoSourcePtr source, FrameSpan interval)
	{
		this->source = source;
		this->interval = interval;
		Reset();
	}

	virtual ~IntervalVideoFilter()
	{
	}

	virtual void Reset()
	{
		currentFrameNumber = 0;
	}

	virtual bool Next()
	{
		currentFrameNumber++;

		if(currentFrameNumber == 1) // need init?
		{
			if(!source->Seek(interval.From))
			{
				return false;
			}
		}

		if(source->GetFrameNumber() < interval.To)
		{
			return source->Next();
		}

		return false;
	}

	virtual Frame GetFrame()
	{
		return source->GetFrame();
	}

	virtual int GetFrameNumber()
	{
		return currentFrameNumber;
	}

	int GetOriginalFrameNumber()
	{
		return source->GetFrameNumber();
	}

	virtual int GetLength()
	{
		if(interval.From > source->GetLength())
		{
			return 0;
		}

		return MIN(interval.To, source->GetLength()) - interval.From + 1;
	}
};

typedef cv::Ptr<IntervalVideoFilter> IntervalVideoFilterPtr;


}}}

#endif // INTERVALVIDEOFILTER_H
