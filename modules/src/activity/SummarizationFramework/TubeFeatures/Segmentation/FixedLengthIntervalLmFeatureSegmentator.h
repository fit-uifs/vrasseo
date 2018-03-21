#pragma once
#ifndef FIXEDLENGTHINTERVALLMFEATURESEGMENTATOR_H
#define FIXEDLENGTHINTERVALLMFEATURESEGMENTATOR_H

#include "LmFeatureSegmentator.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Segmentation
{


class FixedLengthIntervalLmFeatureSegmentator : public LmFeatureSegmentator
{
protected:
	virtual IntervalCollectionPtr internalSegment(Data & data, Idxs & localMaxima)
	{
		IntervalCollectionPtr intervals = new IntervalCollection();
		
		unsigned int segmentStartOffset = segmentLength/2;
		int segmentMaxEnd = data.cols-1;
		
		for(unsigned i = 0; i < localMaxima.size(); i++)
		{
			int start;
			int end;

			int frameNo = localMaxima[i];

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
			
			Interval interv(start, end);
			intervals->push_back(interv);
		}

		return intervals;
	}

public:
	FixedLengthIntervalLmFeatureSegmentator(unsigned int segmentLength, int maxSegmentsCount = -1) : LmFeatureSegmentator(segmentLength, maxSegmentsCount)
	{
	}

	virtual ~FixedLengthIntervalLmFeatureSegmentator()
	{
	}
};


typedef cv::Ptr<FixedLengthIntervalLmFeatureSegmentator> FixedLengthIntervalLmFeatureSegmentatorPtr;


}}}

#endif // FIXEDLENGTHINTERVALLMFEATURESEGMENTATOR_H
