#pragma once
#ifndef BUFFEREDVIDEOSOURCE_H
#define BUFFEREDVIDEOSOURCE_H

#include "IVideoSource.h"

namespace SummarizationFramework
{
namespace Model
{


class BufferedVideoSource : public IVideoSource
{
private:
	typedef std::vector<Frame> Buffer;

	IVideoSourcePtr unbufferedSource;	
	Buffer buffer;
	size_t bufferSize;
	size_t bufferPosition;
	FramePosition currentFrameNumber;
	FramePosition firstBufferFrame;
	FramePosition lastBufferFrame;

protected:
	virtual bool seek(int frame)
	{
		if(isInBuffer(frame))
		{
			currentFrameNumber = frame;
			bufferPosition = getBufferPosition(frame);

			return true;
		}
		else
		{
			if(!unbufferedSource->Seek(frame))
			{
				return false;
			}

			resetBuffer();
			buffer.push_back(unbufferedSource->GetFrame().clone());
			firstBufferFrame = frame;
			lastBufferFrame = frame;
			bufferPosition = 0;
			currentFrameNumber = frame;

			return true;
		}
	}

	void resetBuffer()
	{
		buffer.clear();
		firstBufferFrame = -1;
		lastBufferFrame = -1;
		bufferPosition = 0;
	}

	void initBuffer(size_t size)
	{
		bufferSize = size;
		resetBuffer();
	}

	bool isInBuffer(int frameNumber)
	{
		return lastBufferFrame != -1 && frameNumber >= firstBufferFrame && frameNumber <= lastBufferFrame;
	}

	void fillBuffer(int frameNumber, Frame frame)
	{
		if(isInBuffer(frameNumber))
		{
			bufferPosition = getBufferPosition(frameNumber);
		}
		else if(lastBufferFrame == frameNumber-1)
		{
			if(buffer.size() >= bufferSize)
			{
				buffer.erase(buffer.begin(), buffer.begin()+1);				
				firstBufferFrame++;
			}

			buffer.push_back(frame);
			bufferPosition = buffer.size() - 1;
			lastBufferFrame = frameNumber;
		}
		else
		{
			resetBuffer();
			buffer.push_back(frame);
			firstBufferFrame = frameNumber;
			lastBufferFrame = frameNumber;
			bufferPosition = 0;
		}

		currentFrameNumber = frameNumber;
	}

	size_t getBufferPosition(int frameNumber)
	{
		return frameNumber - firstBufferFrame;
	}

public:
	BufferedVideoSource(IVideoSourcePtr unbufferedSource, size_t bufferSize)
	{
		CV_Assert(unbufferedSource != NULL);
		CV_Assert(bufferSize > 1);

		this->currentFrameNumber = 0;
		this->unbufferedSource = unbufferedSource;
		initBuffer(bufferSize);
	}

	virtual ~BufferedVideoSource()
	{
	}

	virtual void Reset()
	{
		unbufferedSource->Reset();
		resetBuffer();
	}

	virtual bool Next()
	{
		currentFrameNumber++;

		if(isInBuffer(currentFrameNumber))
		{
			bufferPosition = getBufferPosition(currentFrameNumber);
			return true;
		}
		else if(currentFrameNumber == unbufferedSource->GetFrameNumber()+1)
		{
			if(!unbufferedSource->Next())
			{
				return false;
			}

			fillBuffer(currentFrameNumber, unbufferedSource->GetFrame().clone());

			return true;
		}
		else
		{
			if(!unbufferedSource->Seek(currentFrameNumber))
			{
				return false;
			}

			fillBuffer(currentFrameNumber, unbufferedSource->GetFrame().clone());

			return true;
		}
	}

	virtual Frame GetFrame()
	{
		return buffer[bufferPosition];
	}

	virtual int GetFrameNumber()
	{
		return currentFrameNumber;
	}

	virtual int GetLength()
	{
		return unbufferedSource->GetLength();
	}

	int GetBufferSize()
	{
		return bufferSize;
	}
};

typedef cv::Ptr<BufferedVideoSource> BufferedVideoSourcePtr;


}}

#endif // BUFFEREDVIDEOSOURCE_H
