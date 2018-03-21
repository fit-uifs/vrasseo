#pragma once
#ifndef ITUBEFEATURESEGMENTATOR_H
#define ITUBEFEATURESEGMENTATOR_H

#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include "../Features/ITubeFeature.h"
#include "../../Model/TubeCollection.h"
#include "../../Presentation/FrameWindow.h"
#include "../../Presentation/MatrixPlotter.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Segmentation
{


class ITubeFeatureSegmentator
{
public:
	typedef cv::Vec2i Interval;
	typedef std::vector<Interval> IntervalCollection;
	typedef cv::Ptr<IntervalCollection> IntervalCollectionPtr;

private:
	struct MaximaSortPred {
		bool operator()(const cv::Vec2i & A, const cv::Vec2i & B)
		{
			return A[0] <= B[0];
		}
	};

	void mergeIntervals(IntervalCollectionPtr intervals, int threshold, bool presorted = false)
	{
		if(!presorted)
		{
			std::sort(intervals->begin(), intervals->end(), MaximaSortPred());
		}

		IntervalCollection newIntervals;

		size_t i = 0;
		while(i < intervals->size())
		{
			Interval base = (*intervals)[i];
			i++;

			while(i < intervals->size() && (*intervals)[i][0] - base[1] < threshold)
			{
				base[1] = (*intervals)[i][1];
				i++;
			}

			newIntervals.push_back(base);
		}

		intervals->clear();
		intervals->assign(newIntervals.begin(), newIntervals.end());
	}

	bool doMergeIntervals;
	int mergeIntervalsThreshold;

protected:
	virtual IntervalCollectionPtr internalSegmentIntervals(Features::ITubeFeature::FeatureData data) = 0;

public:	
	ITubeFeatureSegmentator()
	{
		SetIntervalMerging(false);
	}

	virtual ~ITubeFeatureSegmentator()
	{
	}

	IntervalCollectionPtr SegmentIntervals(Features::ITubeFeature::FeatureData data)
	{
		IntervalCollectionPtr intervals = internalSegmentIntervals(data);

		if(doMergeIntervals)
		{
			mergeIntervals(intervals, mergeIntervalsThreshold);
		}

		/* VISUALISATION */

		/*
		cv::Mat densityMat = cv::Mat::zeros(1, data.cols, CV_64F);
		for(IntervalCollection::iterator it = intervals->begin(); it != intervals->end(); it++)
		{
			Interval interv = *it;
			for(int i = interv[0]-1; i < interv[1]; i++)
			{
				densityMat.at<double>(0, i) = 1.0;
			}
		}

		Presentation::MatrixPlotter plotter;
		cv::Mat frame = plotter.Plot(densityMat);
		plotter.Plot("density.jpg", densityMat);

		Presentation::FrameWindow window("video coverage");
		window.ShowAndWait(frame, 1);
		*/

		/* VISUALISATION */

		return intervals;
	}

	Model::TubeCollectionPtr SegmentTubes(Features::ITubeFeature::FeatureData data, Model::TubePtr originalTube)
	{
		IntervalCollectionPtr intervals = SegmentIntervals(data);
		Model::TubeCollectionPtr tubes = new Model::TubeCollection();
		
		for(IntervalCollection::iterator it = intervals->begin(); it != intervals->end(); it++)
		{
			Interval interval = *it;
			FrameSpan span = originalTube->GetFrameSpan();
			
			Model::TubePtr tube = new Model::Tube(NULL, FrameSpan(interval[0]+span.From, interval[1]+span.From-1), originalTube->GetFileName());
			tubes->Insert(tube);
		}

		return tubes;
	}

	void SetIntervalMerging(bool enabled, int threshold = 0)
	{
		CV_Assert(threshold >= 0);
		doMergeIntervals = enabled;
		mergeIntervalsThreshold = threshold;
	}
};


typedef cv::Ptr<ITubeFeatureSegmentator> ITubeFeatureSegmentatorPtr;


}}}

#endif // ITUBEFEATURESEGMENTATOR_H
