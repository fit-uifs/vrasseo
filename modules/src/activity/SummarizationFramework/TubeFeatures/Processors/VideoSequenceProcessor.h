#pragma once
#ifndef VIDEOSEQUENCEPROCESSOR_H
#define VIDEOSEQUENCEPROCESSOR_H

#include "TubeProcessor.h"
#include "../../Model/VideoSequence.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Processors
{


class VideoSequenceProcessor : public IProcessor
{
public:
	VideoSequenceProcessor() : IProcessor()
	{
	}

	VideoSequenceProcessor(FeatureExtractorsPtr extractors) : IProcessor(extractors)
	{
	}

	virtual ~VideoSequenceProcessor()
	{
	}

	TubeFeaturesPtr ProcessVideoSequence(Model::IVideoSourcePtr sequence)
	{
		int length = sequence->GetLength();

		ResetExtractors(extractors, length);

		FrameMask mask;
		cv::Rect boundingBox;

		sequence->Seek(0);
		while(sequence->Next() && !isStopRequested())
		{
			Frame frame = sequence->GetFrame();

			if(mask.empty())
			{
				mask = FrameMask::zeros(frame.rows, frame.cols, CV_8UC3);
				mask.setTo(255);
				boundingBox = cv::Rect(0, 0, frame.cols, frame.rows);
			}

			FeedExtractors(extractors, frame, mask, boundingBox);			
		}

		return FinishExtractors(extractors);
	}
};


typedef cv::Ptr<VideoSequenceProcessor> VideoSequenceProcessorPtr;


}}}

#endif // VIDEOSEQUENCEPROCESSOR_H
