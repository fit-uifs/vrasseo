#pragma once
#ifndef VIDEOSEQUENCE_H
#define VIDEOSEQUENCE_H

#include <opencv2/highgui/highgui.hpp>
#include "../CommonTypes.h"
#include "IVideoSource.h"

namespace SummarizationFramework
{
namespace Model
{


class VideoSequence : public IVideoSource
{
private:
    cv::VideoCapture *capture;
	Frame frame;
	std::string filename;
	//static int instcount;

	virtual void lazyCreateCapture()
	{
		if(capture == NULL)
		{
			capture = new cv::VideoCapture(filename);
			//std::cout << ">>VideoSequence::open \"" << filename << "\" (" << ++instcount << ")\n";
		}
	}

protected:
	virtual bool seek(int frame)
	{
		lazyCreateCapture();

		if(frame == GetFrameNumber())
		{
			return true;
		}
		
		int diff = frame - GetFrameNumber();

		if(diff < 0 || diff > 2)
		{
			capture->set(CV_CAP_PROP_POS_FRAMES, frame-1);
		}
		
		while(capture->get(CV_CAP_PROP_POS_FRAMES) < frame-1)
		{
			capture->grab();
		}

		Next();

		return capture->get(CV_CAP_PROP_POS_FRAMES) == frame;
	}

public:
	VideoSequence(std::string filename)
	{
		this->capture = NULL;
		this->filename = filename;
	}

	virtual ~VideoSequence()
	{
		if(capture != NULL)
		{
			//std::cout << ">>VideoSequence::close \"" << filename << "\" (" << --instcount << ")\n";

            /*
			if(capture->isOpened())
			{
				capture->release();
			}
            */

            //delete capture;
            //capture = NULL;

            capture->release(); // TODO/
		}
	}

	virtual void Reset()
	{
		seek(0);
	}

	virtual bool Next()
	{
		lazyCreateCapture();

		if(capture->grab())
		{
			return capture->retrieve(frame);
		}

		return false;
	}

	virtual Frame GetFrame()
	{
		return frame;
	}

	virtual int GetFrameNumber()
	{
		lazyCreateCapture();
		return (int)capture->get(CV_CAP_PROP_POS_FRAMES);
	}

	virtual int GetLength()
	{
		lazyCreateCapture();
		return (int)capture->get(CV_CAP_PROP_FRAME_COUNT);
	}

	std::string GetFilename()
	{
		return filename;
	}

	double GetFps()
	{
		lazyCreateCapture();
		return capture->get(CV_CAP_PROP_FPS);
	}

	FrameSize GetFrameSize()
	{
		lazyCreateCapture();
		return FrameSize((int)capture->get(CV_CAP_PROP_FRAME_WIDTH), (int)capture->get(CV_CAP_PROP_FRAME_HEIGHT));
	}
	
	double GetProperty(int prop)
	{
        lazyCreateCapture();
        return capture->get(prop);
	}
	
	void forceCreateCapture()
	{
		lazyCreateCapture();
	}
};

//int VideoSequence::instcount = 0;

typedef cv::Ptr<VideoSequence> VideoSequencePtr;


}}

#endif // VIDEOSEQUENCE_H
