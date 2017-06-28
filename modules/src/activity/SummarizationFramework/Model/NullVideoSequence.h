#pragma once
#ifndef NULLVIDEOSEQUENCE_H
#define NULLVIDEOSEQUENCE_H

#include <opencv2/highgui/highgui.hpp>
#include "../CommonTypes.h"
#include "../Exceptions.h"
#include "IVideoSource.h"

namespace SummarizationFramework
{
namespace Model
{


class NullVideoSequence : public IVideoSource
{
private:
	Frame frame;
	int length;
	int frameNo;

protected:
	virtual bool seek(int frame)
	{
		if(frame > 0 && (IsInfinite() || frame <= length))
		{
			frameNo = frame;
			return true;
		}
		else
		{
			return false;
		}
	}

public:
	NullVideoSequence(Frame frame, int length = -1)
	{
		SetCustomFrame(frame);
		SetLengthAndReset(length);
	}

	NullVideoSequence(cv::Size size, int length = -1)
	{
		SetEmptyFrame(size);
		SetLengthAndReset(length);
	}

	NullVideoSequence(IVideoSourcePtr videoSource, int length = -1)
	{
		videoSource->Next();
		SetEmptyFrame(videoSource->GetFrame().size());
		SetLengthAndReset((length == -1) ? videoSource->GetLength() : length);
	}

	virtual ~NullVideoSequence()
	{
	}

	virtual void Reset()
	{
		this->frameNo = 0;
	}

	virtual bool Next()
	{
		if(IsInfinite() || frameNo < length)
		{
			frameNo++;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual Frame GetFrame()
	{
		return frame;
	}

	virtual int GetFrameNumber()
	{
		return frameNo;
	}

	virtual int GetLength()
	{
		if(IsInfinite())
		{
			throw new NotSupportedException("Infinite NullVideoSequence cannot return its length.");
		}
		else
		{
			return length;
		}
	}

	bool IsInfinite() const
	{
		return length == -1;
	}

	void SetLengthAndReset(int length = -1)
	{
		CV_Assert(length == -1 || length > 0);
		this->length = length;
		Reset();
	}

	void SetCustomFrame(Frame frame)
	{
		this->frame = frame;
	}

	void SetEmptyFrame(cv::Size size)
	{
		this->frame = Frame::zeros(size, CV_8UC3);
	}
};

typedef cv::Ptr<NullVideoSequence> NullVideoSequencePtr;


}}

#endif // NULLVIDEOSEQUENCE_H
