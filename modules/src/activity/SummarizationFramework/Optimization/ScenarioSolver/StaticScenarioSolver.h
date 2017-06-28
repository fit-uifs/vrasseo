#pragma once
#ifndef STATICSCENARIOSOLVER_H
#define STATICSCENARIOSOLVER_H

#include "IScenarioSolver.h"

namespace SummarizationFramework
{
namespace Optimization
{
namespace ScenarioSolver
{


class StaticScenarioSolver : public IScenarioSolver
{
private:
	int maxTubeLength;

	int computeEffectiveTubeLength(Model::TubePtr tube)
	{
		return (maxTubeLength != -1)? MIN(tube->GetFrameSpan().GetLength(), maxTubeLength) : tube->GetFrameSpan().GetLength();
	}

protected:
	virtual Model::ScenarioPtr internalExecute(Model::TubeCollectionPtr objectTubes)
	{
		Model::Scenario::TubeMappingCollectionPtr mappingCollection = new Model::Scenario::TubeMappingCollection();
		
		//objectTubes->Sort(new Model::TubeCollection::ByOriginalStartSortPredicate());
		
		int currentFrameNo = 1;
		for(Model::TubeCollection::Iterator it = objectTubes->Begin(); it != objectTubes->End(); it++)
		{
			Model::TubePtr tube = *it;

			FramePosition from = currentFrameNo;
			FramePosition to = from + computeEffectiveTubeLength(tube) - 1;

			currentFrameNo = to + 1;

			Model::Scenario::TubeMappingPtr mapping = new Model::Scenario::TubeMapping(tube, from, to);			
			mappingCollection->push_back(mapping);
		}

		return new Model::Scenario(mappingCollection, currentFrameNo);
	}

public:
	StaticScenarioSolver(Model::TubeCollectionPtr objectTubes = NULL) : IScenarioSolver(objectTubes)
	{
		SetMaxTubeLength();
	}

	virtual ~StaticScenarioSolver()
	{
	}

	int GetMaxTubeLength() const
	{
		return maxTubeLength;
	}

	void SetMaxTubeLength(int maxTubeLength = -1)
	{
		CV_Assert(maxTubeLength == -1 || maxTubeLength > 0);

		this->maxTubeLength = maxTubeLength;
	}
};

typedef cv::Ptr<StaticScenarioSolver> StaticScenarioSolverPtr;


}}}

#endif // STATICSCENARIOSOLVER_H
