#pragma once
#ifndef ITUBESEXTRACTOR_H
#define ITUBESEXTRACTOR_H

#include "../../CommonTypes.h"
#include "../../Model/VideoSequence.h"
#include "../../Model/TubeCollection.h"
#include "IOutputHandler.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace TubesExtraction
{


class ITubesExtractor
{
private:
	IOutputHandlerPtr outputHandler;
	bool stopRequested;

protected:
	bool isStopRequested() const
	{
		return this->stopRequested;
	}

public:
	ITubesExtractor(IOutputHandlerPtr outputHandler)
	{
		SetOutputHandler(outputHandler);
		stopRequested = false;
	}

	virtual ~ITubesExtractor()
	{
	}

	IOutputHandlerPtr GetOutputHandler()
	{
		return outputHandler;
	}

	void SetOutputHandler(IOutputHandlerPtr outputHandler)
	{
		this->outputHandler = outputHandler;
	}

	void RequestStop()
	{
		this->stopRequested = true;
	}

	void ProcessVideoSequence(Model::VideoSequencePtr sequence)
	{
		ProcessVideoSource(sequence, sequence->GetFilename());
	}

	virtual void ProcessVideoSource(Model::IVideoSourcePtr video, std::string filename) = 0;
};

typedef cv::Ptr<ITubesExtractor> ITubesExtractorPtr;


}}}

#endif // ITUBESEXTRACTOR_H
