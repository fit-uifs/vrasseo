#pragma once
#ifndef ITUBEFEATUREEXTRACTOR_H
#define ITUBEFEATUREEXTRACTOR_H

#include "ITubeFeature.h"
#include "../../Model/Tube.h"
#include "../../Model/VideoSequence.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Features
{


class ITubeFeatureExtractor
{
public:
	ITubeFeatureExtractor()
	{
	}

	virtual ~ITubeFeatureExtractor()
	{
	}

	virtual void Reset(int size) = 0;

	virtual void Next(Frame frame, FrameMask mask, cv::Rect boundingBox) = 0;

	virtual ITubeFeaturePtr Finish() = 0;
};


typedef cv::Ptr<ITubeFeatureExtractor> ITubeFeatureExtractorPtr;


}}}

#endif // ITUBEFEATUREEXTRACTOR_H
