#pragma once
#ifndef RESAMPLINGVIDEOFILTER_H
#define RESAMPLINGVIDEOFILTER_H

#include <opencv2/imgproc/imgproc.hpp>
#include "../../CommonTypes.h"
#include "../../Model/VideoSequence.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace VideoFilters
{


class ResamplingVideoFilter : public Model::IVideoSource
{
private:
	Model::IVideoSourcePtr source;
	
	Frame currentFrame;
	bool currentFrameReady;

	double fx;
	double fy;

	void prepareCurrentFrame()
	{
		Frame sourceFrame = source->GetFrame();		
		cv::resize(sourceFrame, currentFrame, cv::Size(), fx, fy);		
	}

protected:
	virtual bool seek(int frame)
	{
		currentFrameReady = false;
		return source->Seek(frame);
	}

public:
	// OK
	ResamplingVideoFilter(Model::IVideoSourcePtr source, double fx, double fy)
	{
		this->source = source;
		this->fx = fx;
		this->fy = fy;
	}

	virtual ~ResamplingVideoFilter()
	{
	}

	// OK
	virtual void Reset()
	{
		source->Reset();
		currentFrameReady = false;
	}

	virtual bool Next()
	{
		if(source->Next())
		{
			currentFrameReady = false;
			return true;
		}
		
		return false;
	}

	virtual Frame GetFrame()
	{
		if(!currentFrameReady)
		{
			prepareCurrentFrame();
		}

		return currentFrame;
	}

	virtual int GetFrameNumber()
	{
		return source->GetFrameNumber();
	}

	virtual int GetLength()
	{
		return source->GetLength();
	}
};

typedef cv::Ptr<ResamplingVideoFilter> ResamplingVideoFilterPtr;


}}}

#endif // RESAMPLINGVIDEOFILTER_H
