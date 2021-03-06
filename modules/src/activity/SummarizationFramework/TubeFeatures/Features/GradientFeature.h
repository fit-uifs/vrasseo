#pragma once
#ifndef GRADIENTFEATURE_H
#define GRADIENTFEATURE_H

#include "ITubeFeature.h"
#include "ITubeFeatureExtractor.h"
#include "HistogramAggregator.h"
#include "../../Helpers/Geometry.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Features
{

#define DEBUGVIEW

#define TUBEFEATURE_GRADIENT_SINGLE_ID "gradient-single"
#define TUBEFEATURE_GRADIENT_MULTI_ID "gradient-multi"
#define TUBEFEATURE_GRADIENT_MULTIVECT_ID "gradient-multivect"


class GradientFeature : public ITubeFeature
{
protected:
	class Extractor;
	typedef cv::Ptr<Extractor> ExtractorPtr;

public:
	class SingleExtractor;
	typedef cv::Ptr<SingleExtractor> SingleExtractorPtr;

	class MultiExtractor;
	typedef cv::Ptr<MultiExtractor> MultiExtractorPtr;

	class MultiVectorExtractor;
	typedef cv::Ptr<MultiVectorExtractor> MultiVectorExtractorPtr;

	GradientFeature(FeatureId id) : ITubeFeature(id)
	{
	}

	GradientFeature(FeatureId id, cv::Mat data) : ITubeFeature(id, data)
	{
	}

	virtual ~GradientFeature()
	{
	}
};

typedef cv::Ptr<GradientFeature> GradientFeaturePtr;

class GradientFeature::Extractor : public ITubeFeatureExtractor
{
protected:
	Frame gradPrevFrame;
	FrameMask prevMask;
	cv::Rect prevBoundingBox;
	int tubeLength;

	cv::MatND hist;
	int histChannels;
	int histSize;
	float histMinValue;
	float histMaxValue;
	bool histAccumulate;

	void updatePrev(Frame gradFrame, FrameMask mask, cv::Rect boundingBox)
	{
		gradPrevFrame = gradFrame;
		prevMask = mask;
		prevBoundingBox = boundingBox;
	}

	double getResult(cv::MatND & hist)
	{
		cv::GaussianBlur(hist, hist, cv::Size(1,3), 1.0);

#ifdef DEBUGVIEW
		showHistogram(hist);
#endif

		double minVal = 0;
		double maxVal = 0;
		int minIdx = 0;
		int maxIdx = 0;
		cv::minMaxIdx(hist, &minVal, &maxVal, &minIdx, &maxIdx);

		return (double)maxIdx/histSize;
	}

#ifdef DEBUGVIEW
	void showHistogram(cv::MatND hist)
	{
		int hist_w = 512; int hist_h = 400;
		int bin_w = cvRound( (double) hist_w/histSize );

		cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );
		
		cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
		
		for(int i = 1; i < histSize; i++)
		{
			cv::line(histImage, cv::Point( bin_w*(i-1), hist_h - cvRound(hist.at<float>(i-1)) ) ,
                       cv::Point( bin_w*(i), hist_h - cvRound(hist.at<float>(i)) ),
                       cv::Scalar( rand()%255, rand()%255, rand()%255), 2, 8, 0 );
		}

		cv::imshow("GradientFeature", histImage);
		cv::waitKey(1);
	}
#endif

	virtual void init() = 0;
	virtual void next(cv::MatND & hist) = 0;

public:
	Extractor() : ITubeFeatureExtractor()
	{		
		histChannels = 1;
		histSize = 16;		
		histMinValue = 0;
		histMaxValue = 255;
		histAccumulate = false;
	}

	virtual ~Extractor()
	{
	}

	virtual void Reset(int size)
	{
		gradPrevFrame = Frame();
		prevMask = FrameMask();
		prevBoundingBox = cv::Rect();
		tubeLength = size;

		init();
	}

	virtual void Next(Frame frame, FrameMask mask, cv::Rect boundingBox)
	{
		CV_Assert(!frame.empty());
		CV_Assert(!mask.empty());

		Frame gradCurrentFrame16;
		Frame gradCurrentFrame;
		cv::cvtColor(frame, gradCurrentFrame16, CV_BGR2GRAY);
		cv::Laplacian(gradCurrentFrame16, gradCurrentFrame16, CV_16S);
		
		cv::convertScaleAbs(gradCurrentFrame16, gradCurrentFrame);

		FrameMask currentMask = mask.clone();

		if(gradPrevFrame.empty())
		{
			updatePrev(gradCurrentFrame, currentMask, boundingBox);
			cv::Mat emptyMat;
			next(emptyMat);
			return;
		}

		FrameMask intersectMask(mask.rows, mask.cols, CV_8UC1);
		currentMask.copyTo(intersectMask, prevMask);
		
		FrameMask intersectMask8uc1;
		cv::cvtColor(intersectMask, intersectMask8uc1, CV_BGR2GRAY);
		
		cv::Rect intersectBoundingBox = Helpers::Geometry::RectIntersection(boundingBox, prevBoundingBox);
		Frame diffFrame = gradCurrentFrame - gradPrevFrame;
		
		float c1ranges[] = {histMinValue, histMaxValue};
		const float* histRanges[] = {c1ranges};
		
		cv::calcHist(&diffFrame, 1, 0, intersectMask8uc1, hist, 1, &histSize, histRanges, true, histAccumulate);

		updatePrev(gradCurrentFrame, currentMask, boundingBox);
		next(hist);
	}
};

class GradientFeature::SingleExtractor : public GradientFeature::Extractor
{
protected:
	virtual void init()
	{
	}

	virtual void next(cv::MatND & hist)
	{
	}

public:
	SingleExtractor() : Extractor()
	{
		histAccumulate = true;
	}

	virtual ~SingleExtractor()
	{
	}

	virtual ITubeFeaturePtr Finish()
	{
		cv::Mat result = cv::Mat::zeros(1, 1, CV_64F);
		result.setTo(getResult(hist));
		
		return new GradientFeature(TUBEFEATURE_GRADIENT_SINGLE_ID, result);
	}
};

class GradientFeature::MultiExtractor : public GradientFeature::Extractor
{
protected:
	cv::Mat result;
	int currentIdx;

	virtual void init()
	{
		result = cv::Mat::zeros(1, tubeLength, CV_64F);
		currentIdx = 0;
	}

	virtual void next(cv::MatND & hist)
	{
		result.at<double>(0, currentIdx++) = (hist.empty())? 0 : getResult(hist);
	}

public:
	MultiExtractor() : Extractor()
	{
		histAccumulate = false;
	}

	virtual ~MultiExtractor()
	{
	}

	virtual ITubeFeaturePtr Finish()
	{
		return new GradientFeature(TUBEFEATURE_GRADIENT_MULTI_ID, result.clone());
	}
};

class GradientFeature::MultiVectorExtractor : public GradientFeature::Extractor
{
protected:
	cv::Mat result;
	int currentIdx;

	static const int EXTRA_COLS = 2;
	static const int EXTRA_COL_KMAX_OFFSET = 1;
	static const int EXTRA_COL_KSUM_OFFSET = 2;

	virtual void init()
	{
		result = cv::Mat::zeros(histSize, tubeLength, CV_64F);
		currentIdx = 0;
	}

	virtual void next(cv::MatND & hist)
	{
		for(int i = 0; i < histSize; i++)
		{
			result.at<double>(i, currentIdx) = (hist.empty())? 0 : hist.at<float>(i);
		}

		currentIdx++;
	}

	cv::Mat finalizeResult(cv::Mat data)
	{
		cv::Mat resultFinal = cv::Mat::zeros(EXTRA_COLS + 2*histSize, tubeLength, CV_64F);

		double histStep = (histMaxValue - histMinValue) / histSize;
		
		// indexovani: (hodnota_hist, cislo_snimku)

		for(int r = 0; r < tubeLength; r++)
		{
			double sumK = 0;
			double maxK = 0;

			// 0..(histSize-1) rows => original histogram
			// histSize..(2*histSize-1) => K
			for(int i = 0; i < histSize; i++)
			{				
				double valK = data.at<double>(i, r) * (histMinValue + i*histStep);
				
				resultFinal.at<double>(i, r) = data.at<double>(i, r);
				resultFinal.at<double>(i+histSize, r) = valK;
				
				sumK += valK;
				maxK = MAX(maxK, valK);
			}

			resultFinal.at<double>(resultFinal.rows-EXTRA_COL_KSUM_OFFSET, r) = sumK;
			resultFinal.at<double>(resultFinal.rows-EXTRA_COL_KMAX_OFFSET, r) = maxK;
		}

		return resultFinal;
	}

public:
	MultiVectorExtractor() : Extractor()
	{
		histAccumulate = false;
	}

	virtual ~MultiVectorExtractor()
	{
	}

	virtual ITubeFeaturePtr Finish()
	{
		cv::Mat finalResult = finalizeResult(result);
		return new SimpleSubtractionFeature(TUBEFEATURE_GRADIENT_MULTIVECT_ID, finalResult);
	}
};


}}}

#endif // GRADIENTFEATURE_H
