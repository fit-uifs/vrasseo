#pragma once
#ifndef MOGBACKGROUNDEXTRACTORVIDEOFILTER_H
#define MOGBACKGROUNDEXTRACTORVIDEOFILTER_H

#include <opencv2/video/background_segm.hpp>
#include "../../CommonTypes.h"
#include "../../Model/VideoSequence.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace VideoFilters
{


class MogBackgroundExtractorVideoFilter : public Model::IVideoSource
{
private:
	Model::IVideoSourcePtr source;
	cv::Ptr<cv::BackgroundSubtractorMOG2> mog;

protected:
	virtual bool seek(int frame)
	{
		if(frame == 0)
		{
			Reset();
			return true;
		}
		else
		{
			throw NotSupportedException("MogBackgroundExtractorVideoFilter does not support seek.");
		}
	}

public:
	MogBackgroundExtractorVideoFilter(Model::IVideoSourcePtr source)
	{
		this->source = source;
		this->mog = new cv::BackgroundSubtractorMOG2();
		this->mog->set("detectShadows", 0);
	}

	virtual ~MogBackgroundExtractorVideoFilter()
	{
	}

	virtual void Reset()
	{
		source->Reset();
		this->mog = new cv::BackgroundSubtractorMOG2();
		this->mog->set("detectShadows", 0);
	}

	virtual bool Next()
	{
		if(source->Next())
		{
			FrameMask mask;
			mog->operator()(source->GetFrame(), mask);

			return true;
		}

		return false;
	}

	virtual Frame GetFrame()
	{
		Frame frame;
		mog->getBackgroundImage(frame);
		return frame;
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

typedef cv::Ptr<MogBackgroundExtractorVideoFilter> MogBackgroundExtractorVideoFilterPtr;


}}}

#endif // MOGBACKGROUNDEXTRACTORVIDEOFILTER_H
