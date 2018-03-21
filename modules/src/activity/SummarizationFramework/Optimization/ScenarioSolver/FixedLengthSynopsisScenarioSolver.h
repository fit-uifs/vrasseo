#pragma once
#ifndef FIXEDLENGTHSYNOPSISSCENARIOSOLVER_H
#define FIXEDLENGTHSYNOPSISSCENARIOSOLVER_H

#include "BaseSynopsisScenarioSolver.h"

namespace SummarizationFramework
{
namespace Optimization
{
namespace ScenarioSolver
{


class FixedLengthSynopsisScenarioSolver : public BaseSynopsisScenarioSolver
{
private:
	typedef std::vector<int> TubeShifts;
	typedef cv::Ptr<TubeShifts> TubeShiftsPtr;

	int targetLength;
	TubeShiftsPtr maxShifts;

	int revertTubeIdx;
	bool revertTubeInclusion;
	int revertTubeStart;

	int iterations;
	double temperature;
	double coolingRate;

	OptimizationStatePtr cloneState(OptimizationStatePtr state)
	{
		OptimizationStatePtr stateCopy = new OptimizationState();
		
		for(OptimizationState::iterator it = state->begin(); it != state->end(); it++)
		{
			stateCopy->push_back((*it)->Clone());
		}

		return stateCopy;
	}

	int alterStateInclusion(OptimizationStatePtr state)
	{
		size_t tubeIdx = rand()%state->size();
		TubeStatePtr tubeState = (*state)[tubeIdx];

		revertTubeIdx = tubeIdx;

		tubeState->mapped = !tubeState->mapped;
		revertTubeInclusion = true;

		return tubeIdx;
	}

	int alterStateShift(OptimizationStatePtr state)
	{
		size_t tubeIdx = rand()%state->size();
		TubeStatePtr tubeState = (*state)[tubeIdx];

		revertTubeIdx = tubeIdx;

		revertTubeInclusion = false;
		revertTubeStart = tubeState->start;

		// randomly choose shift increment or decrement
		if(rand()%2)
		{// increase shift
			if(tubeState->start < targetLength-tubeState->tube->GetFrameSpan().GetLength() - shiftStep)
			{
				tubeState->start += shiftStep;
				CV_Assert(tubeState->start < targetLength-tubeState->tube->GetFrameSpan().GetLength());
			}
			else
			{
				return alterStateShift(state);
			}
		}
		else
		{//decrease shift
			if(tubeState->start >= shiftStep)
			{
				tubeState->start -= shiftStep;
				CV_Assert(tubeState->start >= 0);
			}
			else
			{
				return alterStateShift(state);
			}
		}

		return tubeIdx;
	}

	void revertState(OptimizationStatePtr state)
	{
		TubeStatePtr tubeState = (*state)[revertTubeIdx];
		if(revertTubeInclusion)
		{
			tubeState->mapped = !tubeState->mapped;
		}
		else
		{
			tubeState->start = revertTubeStart;
		}
	}

	bool accept(double newEnergy, double oldEnergy)
	{
		// Calculate acceptance probability: declines with time
		double diff = (double) -1.0 * (newEnergy - oldEnergy);
		temperature = temperature * coolingRate;
	
		// Calculate probability of accepting worse solutions
		// This probability declines with time
		double P = exp( diff / temperature );

		if ( P > 1.0 ) return true;
		else
		{
			// Generate random number between 0 and 1
			double p = rand() / double(RAND_MAX);
			return p < P;
		}
	}

protected:
	virtual Model::ScenarioPtr internalExecute(Model::TubeCollectionPtr objectTubes)
	{
		EnergyFunctionEstimatorPtr energyEstimator = new AtcEnergyFunctionEstimator(10, 0.000000025, 0.0000000000001, 10, 10);
		energyEstimator->Init(objectTubes);

		OptimizationStatePtr bestState = createOptimizationState(objectTubes);
		double bestStateEnergy = energyEstimator->Compute(bestState);
		
		for(int globalIters = 10; globalIters > 0; globalIters--)
		{

			OptimizationStatePtr currentState = cloneState(bestState);
			double currentStateEnergy = energyEstimator->Compute(currentState);
		
			iterations = 10000;
			temperature = 100000;
			coolingRate = 0.999;

			for (int i = 0; i < iterations; i++)
			{
				size_t alteredTube;
				double alteredStateEnergy;
			
				// alter inclusion of random tube
				alteredTube = alterStateInclusion(currentState);
				alteredStateEnergy = currentStateEnergy + energyEstimator->ComputeIncrementForInclusionChange(currentState, alteredTube);

				if(accept(alteredStateEnergy, currentStateEnergy))
				{
					currentStateEnergy = alteredStateEnergy;
						
					if(currentStateEnergy < bestStateEnergy)
					{
						bestState = cloneState(currentState);
						bestStateEnergy = currentStateEnergy;
					}
				}
				else
				{
					revertState(currentState);
				}
			
				// alter shift of random tube
				alteredTube = alterStateShift(currentState);
				alteredStateEnergy = currentStateEnergy + energyEstimator->ComputeIncrementForShiftChange(currentState, alteredTube, revertTubeStart);

				if(accept(alteredStateEnergy, currentStateEnergy))
				{
					currentStateEnergy = alteredStateEnergy;

					if(currentStateEnergy < bestStateEnergy)
					{
						bestState = cloneState(currentState);
						bestStateEnergy = currentStateEnergy;
					}
				}
				else
				{
					revertState(currentState);
				}

				if(!(rand()%10))
				{
					//energyEstimator->Compute(currentState);
				}
			}
		}

		//for(OptimizationState::iterator it = bestState->begin(); it != bestState->end(); it++)
		//{
		//	std::cout << "----" << (*it)->start << "\n";
		//}

		int mappedCount = 0;
		for(OptimizationState::iterator it = bestState->begin(); it != bestState->end(); it++)
		{
			if(it->obj->mapped) mappedCount++;
		}

		//std::cout << "mapped " << mappedCount << "/" << bestState->size() << " tubes\n";

		return new Model::Scenario(createMapping(bestState), targetLength);
	}

public:
	FixedLengthSynopsisScenarioSolver(int targetLength, Model::TubeCollectionPtr objectTubes = NULL) : BaseSynopsisScenarioSolver(objectTubes)
	{
		SetLength(targetLength);
	}

	virtual ~FixedLengthSynopsisScenarioSolver()
	{
	}

	void SetLength(int targetLength)
	{
		this->targetLength = targetLength;
	}
};

typedef cv::Ptr<FixedLengthSynopsisScenarioSolver> FixedLengthSynopsisScenarioSolverPtr;


}}}

#endif // FIXEDLENGTHSYNOPSISSCENARIOSOLVER_H
