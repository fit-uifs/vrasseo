#pragma once
#ifndef IOUTPUTHANDLER_H
#define IOUTPUTHANDLER_H

#include "../../Model/Tube.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace TubesExtraction
{


class IOutputHandler
{
protected:
	virtual void takeTube(Model::TubePtr tube) = 0;

public:
	virtual ~IOutputHandler()
	{
	}

	void Take(Model::TubePtr tube)
	{
		// TODO system filtru
		if(tube->GetFrameSpan().To - tube->GetFrameSpan().From < 5)
		{
			return;
		}

		takeTube(tube);
	}

	virtual void Finish()
	{
	}
};

typedef cv::Ptr<IOutputHandler> IOutputHandlerPtr;


}}}

#endif // IOUTPUTHANDLER_H
