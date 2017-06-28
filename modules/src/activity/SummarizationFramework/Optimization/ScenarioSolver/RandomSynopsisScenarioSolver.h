#pragma once
#ifndef RANDOMSYNOPSISSCENARIOSOLVER_H
#define RANDOMSYNOPSISSCENARIOSOLVER_H

#include "IScenarioSolver.h"

namespace SummarizationFramework
{
namespace Optimization
{
namespace ScenarioSolver
{


class RandomSynopsisScenarioSolver : public IScenarioSolver
{
protected:
	virtual Model::ScenarioPtr internalExecute(Model::TubeCollectionPtr objectTubes)
	{
		cv::RNG rng(0xFFFFFFE);
		
		int maxLength = 0;
		for(Model::TubeCollection::Iterator it = objectTubes->Begin(); it != objectTubes->End(); it++)
		{
			Model::TubePtr tube = *it;
			
			int length = tube->GetFrameSpan().To - tube->GetFrameSpan().From + 1;
			maxLength = MAX(maxLength, length);
		}

		Model::Scenario::TubeMappingCollectionPtr mappingCollection = new Model::Scenario::TubeMappingCollection();
		
		for(Model::TubeCollection::Iterator it = objectTubes->Begin(); it != objectTubes->End(); it++)
		{
			Model::TubePtr tube = *it;

			int length = tube->GetFrameSpan().To - tube->GetFrameSpan().From + 1;

			FramePosition from = MAX(1, rng.uniform(0, maxLength-length-1));
			FramePosition to = from + length - 1;

			Model::Scenario::TubeMappingPtr mapping = new Model::Scenario::TubeMapping(tube, from, to);			
			mappingCollection->push_back(mapping);
		}

		return new Model::Scenario(mappingCollection, maxLength);
	}

public:
	RandomSynopsisScenarioSolver(Model::TubeCollectionPtr objectTubes = NULL) : IScenarioSolver(objectTubes)
	{
	}

	virtual ~RandomSynopsisScenarioSolver()
	{
	}
};

typedef cv::Ptr<RandomSynopsisScenarioSolver> RandomSynopsisScenarioSolverPtr;


}}}

#endif // RANDOMSYNOPSISSCENARIOSOLVER_H
