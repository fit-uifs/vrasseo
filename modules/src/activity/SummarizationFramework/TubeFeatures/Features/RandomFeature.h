#pragma once
#ifndef RANDOMFEATURE_H
#define RANDOMFEATURE_H

#include "ITubeFeature.h"
#include "ITubeFeatureExtractor.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Features
{

#define TUBEFEATURE_RANDOM_ID "random"


class RandomFeature : public ITubeFeature
{
public:
	class Extractor;
	typedef cv::Ptr<Extractor> ExtractorPtr;

	RandomFeature() : ITubeFeature(TUBEFEATURE_RANDOM_ID)
	{
	}

	RandomFeature(FeatureData data) : ITubeFeature(TUBEFEATURE_RANDOM_ID, data)
	{
	}

	virtual ~RandomFeature()
	{
	}
};

typedef cv::Ptr<RandomFeature> RandomFeaturePtr;

class RandomFeature::Extractor : public ITubeFeatureExtractor
{
public:
	Extractor() : ITubeFeatureExtractor()
	{
	}

	virtual ~Extractor()
	{
	}

	virtual void Reset(int size)
	{
	}

	virtual void Next(Frame frame, FrameMask mask, cv::Rect boundingBox)
	{
	}

	virtual ITubeFeaturePtr Finish()
	{
		cv::Mat data(1, 1, CV_64F);
		data.at<double>(0,0) = ((double)(rand()%100))/100;
		return new RandomFeature(data);
	}
};


}}}

#endif // RANDOMFEATURE_H
