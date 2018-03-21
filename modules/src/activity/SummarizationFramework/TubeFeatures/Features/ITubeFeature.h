#pragma once
#ifndef ITUBEFEATURE_H
#define ITUBEFEATURE_H

#include <string>
#include "../../CommonTypes.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Features
{

#define TUBEFEATURE_UNKNOWN_ID "unknown"


class ITubeFeature
{
public:
	typedef std::string FeatureId;
	typedef cv::Mat FeatureData;
	typedef cv::Ptr<FeatureData> FeatureDataPtr;

protected:
	FeatureId id;
	FeatureData data;
	
public:
	ITubeFeature(FeatureId id)
	{
		this->id = id;
		this->data = FeatureData();
	}
	
	ITubeFeature(FeatureId id, FeatureData data)
	{
		this->id = id;
		this->data = data;
	}

	virtual ~ITubeFeature()
	{
	}

	FeatureId GetId()
	{
		return id;
	}

	FeatureData GetData()
	{
		return data;
	}

	virtual bool IsEmpty()
	{
		return data.empty();
	}

	virtual bool IsMultiFeature()
	{
		return !data.empty() && data.cols > 1;
	}
};


typedef cv::Ptr<ITubeFeature> ITubeFeaturePtr;


}}}

#endif // ITUBEFEATURE_H
