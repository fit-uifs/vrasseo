#pragma once
#ifndef HISTOGRAMAGGREGATOR_H
#define HISTOGRAMAGGREGATOR_H

#include "../../CommonTypes.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Features
{


class HistogramAggregator
{
public:
	void AddValue(int value, int count = 1)
	{
		// TODO
	}

	int GetResult()
	{
		// TODO
		return 123;
	}

	void Clear()
	{
		// TODO
	}
};

typedef cv::Ptr<HistogramAggregator> HistogramAggregatorPtr;


}}}

#endif // HISTOGRAMAGGREGATOR_H
