#pragma once
#ifndef IVIDEOSOURCE_H
#define IVIDEOSOURCE_H

#include "../CommonTypes.h"

namespace SummarizationFramework
{
namespace Model
{


class IVideoSource
{
protected:
	virtual bool seek(int frame) = 0;

public:
	virtual void Reset() = 0;	
	virtual bool Next() = 0;
	virtual Frame GetFrame() = 0;
	virtual int GetFrameNumber() = 0;
	virtual int GetLength() = 0;

	bool Seek(int frame)
	{
		if(GetFrameNumber() == frame)
		{
			return true;
		}

		if(seek(frame))
		{
			return true;
		}
		else
		{
			if(GetFrameNumber() > frame)
			{
				Reset();
			}

			while(GetFrameNumber() > frame)
			{
				if(!Next())
				{
					return false;
				}
			}

			return true;
		}		
	}

	virtual ~IVideoSource()
	{
	}
};

typedef cv::Ptr<IVideoSource> IVideoSourcePtr;


}}

#endif // IVIDEOSOURCE_H
