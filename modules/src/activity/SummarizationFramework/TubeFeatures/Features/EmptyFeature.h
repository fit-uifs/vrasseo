#pragma once
#ifndef EMPTYFEATURE_H
#define EMPTYFEATURE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ITubeFeature.h"
#include "ITubeFeatureExtractor.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Features
{

#define DEBUGVIEW

#define TUBEFEATURE_UNKNOWN_ID "unknown"


class EmptyFeature : public ITubeFeature
{
public:
	class Extractor;
	typedef cv::Ptr<Extractor> ExtractorPtr;

	EmptyFeature() : ITubeFeature(TUBEFEATURE_UNKNOWN_ID)
	{
		FeatureData data(1, 1, CV_64F);
		data.setTo(0);
		this->data = data;
	}

	virtual ~EmptyFeature()
	{
	}
};

typedef cv::Ptr<EmptyFeature> EmptyFeaturePtr;

class EmptyFeature::Extractor : public ITubeFeatureExtractor
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
#ifdef DEBUGVIEW
		cv::Mat tmpimg;
		frame.copyTo(tmpimg, mask);
		cv::resize(tmpimg, tmpimg, cv::Size(), 0.5, 0.5);
		cv::imshow("EmptyFeature", tmpimg);
		cv::waitKey(1);
#endif

	}

	virtual ITubeFeaturePtr Finish()
	{
		return new EmptyFeature();
	}
};


}}}

#endif // EMPTYFEATURE_H
