#pragma once
#ifndef LMFEATURESEGMENTATOR_H
#define LMFEATURESEGMENTATOR_H

#include <algorithm>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "ITubeFeatureSegmentator.h"
#include "../../Presentation/FrameWindow.h"
#include "../../Presentation/MatrixPlotter.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Segmentation
{


class LmFeatureSegmentator : public ITubeFeatureSegmentator
{
protected:
	typedef std::vector<unsigned> Idxs;
	typedef Features::ITubeFeature::FeatureData Data;

private:
	Idxs detectLocalMaxima(const Data & data)
	{
		CV_Assert(data.rows == 1);
		CV_Assert(data.cols > 0);
		
		Idxs maxima;

		for(unsigned i = 1; i < (unsigned)data.cols-1; i++)
		{
			if(data.at<double>(0, i) > data.at<double>(0, i-1) && data.at<double>(0, i) > data.at<double>(0, i+1))
			{
				maxima.push_back(i);
			}
		}

		return maxima;
	}

	void smoothData(Data & data)
	{
		cv::GaussianBlur(data, data, cv::Size(51, 1), 10.0);
	}

	void showVisualization(Data & data, Idxs & localMaxima)
	{
		cv::Mat maximaMat = cv::Mat::zeros(1, data.cols, CV_64F);
		for(size_t i = 0; i < localMaxima.size(); i++)
		{
			int idx = localMaxima[i];
			maximaMat.at<double>(0, idx) = 1;
		}

		Presentation::MatrixPlotter::MatVector matrices;
		matrices.push_back(maximaMat);
		matrices.push_back(data);

		Presentation::MatrixPlotter::ColorVector colors;
		colors.push_back(cv::Scalar(100, 100, 255));
		colors.push_back(cv::Scalar(0, 0, 0));

		Presentation::MatrixPlotter plotter;
		cv::Mat plot = plotter.Plot(matrices, colors);

		Presentation::FrameWindow window("LmFeatureSegmentator");
		window.ShowAndWait(plot, 0);
	}

	struct MaximaSortPred {
	private:
		Data *data;

	public:
		MaximaSortPred(Data *data)
		{
			this->data = data;
		}

		bool operator()(int i, int j)
		{
			return data->at<double>(0, i) > data->at<double>(0, j);
		}
	};

protected:
	int maxSegmentsCount;
	unsigned int segmentLength;

	virtual IntervalCollectionPtr internalSegment(Data & data, Idxs & localMaxima) = 0;

	virtual IntervalCollectionPtr internalSegmentIntervals(Features::ITubeFeature::FeatureData data)
	{
		Data workingCopyData = data.clone();
		smoothData(workingCopyData);

		Idxs localMaxima = detectLocalMaxima(workingCopyData);
		
		MaximaSortPred maximaSortPred(&workingCopyData);
		std::sort(localMaxima.begin(), localMaxima.end(), maximaSortPred);

		//showVisualization(workingCopyData, localMaxima);

		IntervalCollectionPtr intervals = internalSegment(workingCopyData, localMaxima);

		if(maxSegmentsCount != -1)
		{
			IntervalCollectionPtr newIntervals = new IntervalCollection();

			for(int i = 0; i < (signed)intervals->size() && i < maxSegmentsCount; i++)
			{
				newIntervals->push_back((*intervals)[i]);
			}

			return newIntervals;
		}
		else
		{
			return intervals;
		}
	}

public:
	LmFeatureSegmentator(unsigned int segmentLength, int maxSegmentsCount = -1) : ITubeFeatureSegmentator()
	{
		SetMaxSegmentsCount(maxSegmentsCount);
		SetSegmentLength(segmentLength);
	}

	virtual ~LmFeatureSegmentator()
	{
	}

	void SetMaxSegmentsCount(int maxSegmentsCount = -1)
	{
		CV_Assert(maxSegmentsCount == -1 || maxSegmentsCount > 0);
		this->maxSegmentsCount = maxSegmentsCount;
	}

	void SetSegmentLength(unsigned int segmentLength)
	{
		CV_Assert(segmentLength > 0);
		this->segmentLength = segmentLength;
	}
};


typedef cv::Ptr<LmFeatureSegmentator> LmFeatureSegmentatorPtr;


}}}

#endif // LMFEATURESEGMENTATOR_H
