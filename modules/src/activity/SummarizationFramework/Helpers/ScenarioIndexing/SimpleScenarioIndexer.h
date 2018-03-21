#pragma once
#ifndef SIMPLESCENARIOINDEXER_H
#define SIMPLESCENARIOINDEXER_H

#include "IScenarioIndexer.h"

namespace SummarizationFramework
{
namespace Helpers
{
namespace ScenarioIndexing
{


class SimpleScenarioIndexer : public IScenarioIndexer
{
private:
	bool intersectionNonEmpty(FrameSpan span1, FrameSpan span2)
	{
		FramePosition lowerBound = MAX(span1.From, span2.From);
		FramePosition upperBound = MIN(span1.To, span2.To);
		
		return lowerBound <= upperBound;
	}

	Model::ScenarioPtr scenario;

protected:
	virtual void buildIndex(Model::ScenarioPtr scenario)
	{
		this->scenario = scenario;
	}

public:
	SimpleScenarioIndexer(Model::ScenarioPtr scenario = NULL) : IScenarioIndexer(scenario)
	{
	}

	virtual ~SimpleScenarioIndexer()
	{
	}

	virtual Model::Scenario::TubeMappingCollection GetTubes(FrameSpan span)
	{
		Model::Scenario::TubeMappingCollection intersectingTubes;
		Model::Scenario::TubeMappingCollectionPtr mapping = scenario->GetMapping();

		for(Model::Scenario::TubeMappingCollection::iterator it = mapping->begin(); it != mapping->end(); it++)
		{
			if(intersectionNonEmpty(span, (*it)->GetFrameSpan()))
			{
				intersectingTubes.push_back(*it);
			}
		}

		return intersectingTubes;
	}

	virtual Model::Scenario::TubeMappingCollection GetTubes(FramePosition position)
	{
		return GetTubes(FrameSpan(position, position));
	}
};

typedef cv::Ptr<SimpleScenarioIndexer> SimpleScenarioIndexerPtr;


}}}

#endif // SIMPLESCENARIOINDEXER_H
