#pragma once
#ifndef TIMELAPSEVIDEOFILTER_H
#define TIMELAPSEVIDEOFILTER_H

#include "../../CommonTypes.h"
#include "../../Model/IVideoSource.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace VideoFilters
{


class TimelapseVideoFilter : public Model::IVideoSource
{
private:
	Model::IVideoSourcePtr source;
	Frame currentFrame;
	FramePosition currentFrameNumber;
	int targetLength;
	double skipStep;
	double skipAcc;

protected:
	virtual bool seek(int frame)
	{
		throw NotSupportedException("TimelapseVideoFilter does not support seek.");
	}

public:
	TimelapseVideoFilter(Model::IVideoSourcePtr source, int targetLength)
	{
		int sourceLength = source->GetLength();
		std::cout << "src_len=" << sourceLength << ", tgt_len=" << targetLength << "\n";
		CV_Assert(sourceLength >= targetLength);
		CV_Assert(targetLength > 0);

		this->source = source;
		this->targetLength = targetLength;
		this->skipStep = (double)targetLength / sourceLength;
		
		Reset();
	}

	virtual ~TimelapseVideoFilter()
	{
	}

	virtual void Reset()
	{
		currentFrameNumber = 0;
		skipAcc = 0;
	}

	virtual bool Next()
	{
		if(currentFrameNumber++ >= targetLength)
		{
			return false;
		}

		while(skipAcc < 1.0 && source->Next())
		{
			skipAcc += skipStep;
		}

		currentFrame = source->GetFrame().clone();
		skipAcc -= 1.0;

		return true;
	}

	virtual Frame GetFrame()
	{
		return currentFrame;
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
		return targetLength;
	}
};

typedef cv::Ptr<TimelapseVideoFilter> TimelapseVideoFilterPtr;


}}}

#endif // TIMELAPSEVIDEOFILTER_H
