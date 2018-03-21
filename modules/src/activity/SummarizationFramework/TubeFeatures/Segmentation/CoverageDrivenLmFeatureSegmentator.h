#pragma once
#ifndef COVERAGEDRIVENLMFEATURESEGMENTATOR_H
#define COVERAGEDRIVENLMFEATURESEGMENTATOR_H

#include "LmFeatureSegmentator.h"
#include "../../Helpers/Strings.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Segmentation
{


class CoverageDrivenLmFeatureSegmentator : public LmFeatureSegmentator
{
private:
	double *dataBuff;
	double *coverageBuff;
	double *productBuff;
	int bufferSize;

#ifdef VISUAL_DEBUG
	void showBuffer(const std::string & name, double *arr, int size, int point = -1)
	{		
		CV_Assert(point == -1 || (point >= 0 && point < size));
		
		static int xxxx = 0;

		cv::Mat matData(1, size, CV_64F);
		for(int i = 0; i < size; i++)
		{
			matData.at<double>(0, i) = arr[i];
		}

		Presentation::MatrixPlotter::MatVector matrices;
		Presentation::MatrixPlotter::ColorVector colors;

		matrices.push_back(matData);
		colors.push_back(cv::Scalar(0, 0, 0));
		
		if(point >= 0)
		{
			cv::Mat matPoint = cv::Mat::zeros(1, size, CV_64F);
			matPoint.at<double>(0, point) = 1.0;
			
			matrices.push_back(matPoint);
			colors.push_back(cv::Scalar(0, 0, 255));
		}
				
		Presentation::MatrixPlotter plotter;
		Frame frame = plotter.Plot(matrices, colors);
		
		plotter.Plot(std::string("plot")+Helpers::Strings::toString(xxxx++)+".jpg",matrices, colors);
		
		//Presentation::FrameWindow window(name);
		//window.ShowAndWait(frame, 1);
	}
#endif

	void initBuffers(Data & dataMat)
	{
		releaseBuffers();
		
		bufferSize = dataMat.cols;

		dataBuff = new double[bufferSize];
		coverageBuff = new double[bufferSize];
		productBuff = new double[bufferSize];

		for(int i = 0; i < bufferSize; i++)
		{
			dataBuff[i] = dataMat.at<double>(0, i);
			coverageBuff[i] = 1;
		}

#ifdef VISUAL_DEBUG
		showBuffer("coverage", coverageBuff, bufferSize);
#endif
	}

	void releaseBuffers()
	{
		if(dataBuff != NULL)
		{
			delete dataBuff;
			dataBuff = NULL;
		}

		if(coverageBuff != NULL)
		{
			delete coverageBuff;
			coverageBuff = NULL;
		}

		if(productBuff != NULL)
		{
			delete productBuff;
			productBuff = NULL;
		}
	}

	int findStrongestMaximum(double *arr, int size)
	{
		int maxIdx = -1;
		double maxVal = DBL_MIN;

		for(int i = 1; i < size-1; i++)
		{
			if(arr[i] > arr[i-1] && arr[i] > arr[i+1])
			{
				if(arr[i] > maxVal)
				{
					maxIdx = i;
					maxVal = arr[i];
				}
			}
		}

#ifdef VISUAL_DEBUG		
		showBuffer("maximum is found", arr, size, maxIdx);
#endif

		return maxIdx;
	}

	Interval createSegment(int frameNo)
	{
		unsigned int segmentStartOffset = segmentLength/2;
		int segmentMaxEnd = bufferSize-1;

		int start;
		int end;

		if((unsigned)frameNo < segmentStartOffset)
		{
			start = 0;
			end = MIN(segmentLength, (unsigned)segmentMaxEnd);
		}
		else
		{
			start = frameNo-segmentStartOffset;
			end = MIN(start+segmentLength, (unsigned)segmentMaxEnd);
		}

		return Interval(start, end);
	}

	void updateCoverage(Interval interval)
	{
		for(int i = interval[0]; i < interval[1]; i++)
		{			
			coverageBuff[i] = 0;
		}

		cv::Mat matToBlur(1, bufferSize, CV_64F, coverageBuff);
		
		matToBlur = matToBlur.clone();
		cv::GaussianBlur(matToBlur, matToBlur, cv::Size(351, 1), 100.0);

		for(int i = 0; i < bufferSize; i++)
		{
			coverageBuff[i] = coverageBuff[i]*matToBlur.at<double>(0, i);
		}

#ifdef VISUAL_DEBUG
		showBuffer("coverage", coverageBuff, bufferSize);
#endif
	}

	void updateProduct()
	{
		for(int i = 0; i < bufferSize; i++)
		{
			productBuff[i] = coverageBuff[i]*dataBuff[i];
		}
	}

protected:
	virtual IntervalCollectionPtr internalSegment(Data & data, Idxs & localMaxima)
	{
		CV_Assert(maxSegmentsCount > 0);
		
		IntervalCollectionPtr intervals = new IntervalCollection();
		
		initBuffers(data);

		try
		{
			while(intervals->size() < (unsigned)maxSegmentsCount)
			{			
				updateProduct();

				int maxIdx = findStrongestMaximum(productBuff, bufferSize);
				if(maxIdx < 0)
				{
					break;
				}
				
				Interval interval = createSegment(maxIdx);

				intervals->push_back(interval);
				updateCoverage(interval);
			}
		}
		catch(...)
		{
			releaseBuffers();
			throw;
		}

		releaseBuffers();

		return intervals;
	}

public:
	CoverageDrivenLmFeatureSegmentator(unsigned int segmentLength, int maxSegmentsCount = -1) : LmFeatureSegmentator(segmentLength, maxSegmentsCount)
	{
		dataBuff = NULL;
		coverageBuff = NULL;
		productBuff = NULL;
	}

	virtual ~CoverageDrivenLmFeatureSegmentator()
	{
		releaseBuffers();
	}
};


typedef cv::Ptr<CoverageDrivenLmFeatureSegmentator> CoverageDrivenLmFeatureSegmentatorPtr;


}}}

#endif // COVERAGEDRIVENLMFEATURESEGMENTATOR_H
