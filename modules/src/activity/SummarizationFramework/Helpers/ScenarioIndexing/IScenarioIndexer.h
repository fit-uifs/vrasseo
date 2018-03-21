#pragma once
#ifndef ISCENARIOINDEXER_H
#define ISCENARIOINDEXER_H

#include <vector>
#include "../../CommonTypes.h"
#include "../../Model/Scenario.h"

namespace SummarizationFramework
{
namespace Helpers
{
namespace ScenarioIndexing
{


class IScenarioIndexer
{
private:
	Model::ScenarioPtr scenario;

protected:
	virtual void buildIndex(Model::ScenarioPtr scenario) = 0;

public:
	IScenarioIndexer(Model::ScenarioPtr scenario = NULL)
	{
		this->scenario = scenario;

		if(scenario != NULL)
		{
			Rebuild();
		}
	}

	virtual ~IScenarioIndexer()
	{
	}

	void SetScenario(Model::ScenarioPtr scenario)
	{
		this->scenario = scenario;
		Rebuild();
	}

	void Rebuild()
	{
		buildIndex(scenario);
	}

	virtual Model::Scenario::TubeMappingCollection GetTubes(FramePosition position) = 0;
	
	virtual Model::Scenario::TubeMappingCollection GetTubes(FrameSpan span) = 0;
};

typedef cv::Ptr<IScenarioIndexer> IScenarioIndexerPtr;


}}}

#endif // ISCENARIOINDEXER_H
