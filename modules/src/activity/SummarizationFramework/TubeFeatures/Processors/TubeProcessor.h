#pragma once
#ifndef TUBEPROCESSOR_H
#define TUBEPROCESSOR_H

#include "IProcessor.h"

namespace SummarizationFramework
{
namespace TubeFeatures
{
namespace Processors
{


class TubeProcessor : public IProcessor
{
protected:
	Model::VideoSequencePtr sequence;

public:
	TubeProcessor() : IProcessor()
	{
		sequence = NULL;
	}

	TubeProcessor(FeatureExtractorsPtr extractors) : IProcessor(extractors)
	{
		sequence = NULL;
	}

	virtual ~TubeProcessor()
	{
	}

	TubeFeaturesPtr ProcessTube(Model::TubePtr tube)
	{
		if(sequence == NULL || sequence->GetFilename() != tube->GetFileName())
		{
			sequence = new Model::VideoSequence(tube->GetFileName());
		}

		if(sequence->GetFrameNumber() != tube->GetFrameSpan().From)
		{
			sequence->Seek(tube->GetFrameSpan().From);
		}

		ResetExtractors(extractors, tube->GetFrames()->size());

		size_t tubePos = 0;
		while(sequence->Next() && tubePos < tube->GetFrames()->size() && !isStopRequested())
		{
			Frame frame = sequence->GetFrame();
			FrameMask mask = (*(tube->GetFrames()))[tubePos]->GetData()->GetMask();
			cv::Rect boundingBox = (*(tube->GetFrames()))[tubePos]->GetBoundingBox();

			FeedExtractors(extractors, frame, mask, boundingBox);			
			tubePos++;
		}

		CV_Assert(tubePos == tube->GetFrames()->size());

		return FinishExtractors(extractors);
	}

	TubeFeaturesCollectionPtr ProcessCollection(Model::TubeCollectionPtr collection)
	{
		TubeFeaturesCollectionPtr collectionFeatures = new TubeFeaturesCollection();

		size_t current = 0;
		size_t total = collection->Count();

		for(Model::TubeCollection::Iterator it = collection->Begin(); it != collection->End() && !isStopRequested(); it++)
		{
			TubeFeaturesPtr tubeFeatures = ProcessTube(*it);
			collectionFeatures->push_back(tubeFeatures);
		}

		return collectionFeatures;
	}
};


typedef cv::Ptr<TubeProcessor> TubeProcessorPtr;


}}}

#endif // TUBEPROCESSOR_H
