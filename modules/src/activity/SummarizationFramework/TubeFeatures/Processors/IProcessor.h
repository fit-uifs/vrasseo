#pragma once
#ifndef IPROCESSOR_H
#define IPROCESSOR_H

#include "../Features/ITubeFeature.h"
#include "../Features/ITubeFeatureExtractor.h"
#include "../../Model/Tube.h"
#include "../../Model/TubeCollection.h"
#include "../../Model/VideoSequence.h"
#include <vector>

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Processors
{


class IProcessor
{
public:
	typedef std::vector<Features::ITubeFeatureExtractorPtr> FeatureExtractors;
	typedef cv::Ptr<FeatureExtractors> FeatureExtractorsPtr;

	typedef std::vector<Features::ITubeFeaturePtr> TubeFeatures;
	typedef cv::Ptr<TubeFeatures> TubeFeaturesPtr;

	typedef std::vector<TubeFeaturesPtr> TubeFeaturesCollection;
	typedef cv::Ptr<TubeFeaturesCollection> TubeFeaturesCollectionPtr;

private:
	bool stopRequested;

protected:
	FeatureExtractorsPtr extractors;

	bool isStopRequested() const
	{
		return stopRequested;
	}

public:
	static void ResetExtractors(FeatureExtractorsPtr extractors, int size)
	{
		for(FeatureExtractors::iterator it = extractors->begin(); it != extractors->end(); it++)
		{
			(*it)->Reset(size);
		}
	}

	static void FeedExtractors(FeatureExtractorsPtr extractors, Frame frame, FrameMask mask, cv::Rect boundingBox)
	{
		for(FeatureExtractors::iterator it = extractors->begin(); it != extractors->end(); it++)
		{
			(*it)->Next(frame, mask, boundingBox);
		}
	}

	static TubeFeaturesPtr FinishExtractors(FeatureExtractorsPtr extractors)
	{
		TubeFeaturesPtr features = new TubeFeatures();

		for(FeatureExtractors::iterator it = extractors->begin(); it != extractors->end(); it++)
		{
			Features::ITubeFeaturePtr tubeFeature = (*it)->Finish();
			features->push_back(tubeFeature);
		}

		return features;
	}

	IProcessor()
	{
		stopRequested = false;
		SetFeatureExtractors(new FeatureExtractors());
	}

	IProcessor(FeatureExtractorsPtr extractors)
	{
		SetFeatureExtractors(extractors);
	}

	virtual ~IProcessor()
	{
	}

	void RequestStop()
	{
		stopRequested = true;
	}

	void SetFeatureExtractors(FeatureExtractorsPtr extractors)
	{
		CV_Assert(extractors != NULL);
		this->extractors = extractors;
	}

	FeatureExtractorsPtr GetFeatureExtractors()
	{
		return extractors;
	}
};


typedef cv::Ptr<IProcessor> IProcessorPtr;


}}}

#endif // IPROCESSOR_H
