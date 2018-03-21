#pragma once
#ifndef BASESYNOPSISSCENARIOSOLVER_H
#define BASESYNOPSISSCENARIOSOLVER_H

#include <map>
#include <vector>
#include <cmath>
#include <opencv2/core/core.hpp>
#include "IScenarioSolver.h"
#include "../../Helpers/Geometry.h"

namespace SummarizationFramework
{
namespace Optimization
{
namespace ScenarioSolver
{


class BaseSynopsisScenarioSolver : public IScenarioSolver
{
protected:
	class TubeState;
	typedef cv::Ptr<TubeState> TubeStatePtr;

	class TubeState
	{
	public:
		Model::TubePtr tube;
		bool mapped;
		int start;

		TubeState(Model::TubePtr tube, bool mapped = true, int start = 0)
		{
			this->tube = tube;
			this->mapped = mapped;
			this->start = start;
		}

		TubeStatePtr Clone()
		{;
			return new TubeState(tube, mapped, start);
		}
	};
	
	typedef std::vector<TubeStatePtr> OptimizationState;
	typedef cv::Ptr<OptimizationState> OptimizationStatePtr;

	class EnergyFunctionEstimator
	{
	public:
		virtual void Init(Model::TubeCollectionPtr objectTubes) = 0;
		virtual double Compute(OptimizationStatePtr state) = 0;
		virtual double ComputeIncrementForInclusionChange(OptimizationStatePtr state, size_t idx) = 0;
		virtual double ComputeIncrementForShiftChange(OptimizationStatePtr state, size_t idx, int oldValue) = 0;
	};

	typedef cv::Ptr<EnergyFunctionEstimator> EnergyFunctionEstimatorPtr;

	class AtcEnergyFunctionEstimator : public EnergyFunctionEstimator
	{
	private:
		typedef std::map<Model::TubePtr, double> ActivityCostLookup;
		typedef cv::Ptr<ActivityCostLookup> ActivityCostLookupPtr;

		typedef std::vector<double> ShiftCosts;
		typedef cv::Ptr<ShiftCosts> ShiftCostsPtr;
		typedef std::map<Model::TubePtr, ShiftCostsPtr> TubeShiftCosts;
		typedef cv::Ptr<TubeShiftCosts> TubeShiftCostsPtr;
		typedef std::map<Model::TubePtr, TubeShiftCostsPtr> CollisionCostLookup;
		typedef cv::Ptr<CollisionCostLookup> CollisionCostLookupPtr;

		typedef std::pair<Model::TubePtr, Model::TubePtr> TubesPair;
		typedef std::map<TubesPair, double> TemporalConsistencyCostLookup;
		typedef cv::Ptr<TemporalConsistencyCostLookup> TemporalConsistencyCostLookupPtr;

		ActivityCostLookupPtr activityCostLookup;
		CollisionCostLookupPtr collisionCostLookup;
		TemporalConsistencyCostLookupPtr temporalConsistencyCostLookup;
		
		double alpha;
		double beta;
		double sigma_time;
		double sigma_space;
		int shiftStep;

		double computeTubeActivityCost(Model::TubePtr tube)
		{
			double cost = 0;

			for(Model::TubeFrameCollection::iterator it = tube->GetFrames()->begin(); it != tube->GetFrames()->end(); it++)
			{
				cost += it->obj->GetBoundingBox().area(); // TODO ne obsah bboxu, ale skutecna plocha masky
			}

			FrameMask mask = tube->GetFrames()->at(0)->GetData()->GetMask();

			return cost / (double)(mask.cols * mask.rows);
		}

		void initActivityCostLookup(Model::TubeCollectionPtr objectTubes)
		{
			activityCostLookup = new ActivityCostLookup();

			for(Model::TubeCollection::Iterator it = objectTubes->Begin(); it != objectTubes->End(); it++)
			{
				Model::TubePtr tube = *it;
				double cost = computeTubeActivityCost(tube);
				(*activityCostLookup)[tube] = cost;
			}
		}

		double getFramesPairCollisionCost(Model::TubeFramePtr frame1, Model::TubeFramePtr frame2)
		{
			cv::Rect intersection = Helpers::Geometry::RectIntersection(frame1->GetBoundingBox(), frame2->GetBoundingBox());
			return intersection.area(); // TODO ne obsah pruniku bboxu, ale skutecna plocha pruniku masek
		}

		double getTubesPairCollisionCost(Model::TubePtr tube1, Model::TubePtr tube2, int tube2shift)
		{
			double cost = 0;
			
			Model::TubeFrameCollectionPtr frames1 = tube1->GetFrames();			
			Model::TubeFrameCollectionPtr frames2 = tube2->GetFrames();
						
			size_t pos1 = tube2shift;
			size_t pos2 = 0;
			
			for(pos1, pos2; pos1 < frames1->size() && pos2 < frames2->size(); pos1++, pos2++)
			{
				cost += getFramesPairCollisionCost((*frames1)[pos1], (*frames2)[pos2]);
			}

			return cost;
		}

		void initCollisionCostLookup(Model::TubeCollectionPtr objectTubes)
		{
			collisionCostLookup = new CollisionCostLookup();

			for(Model::TubeCollection::Iterator it = objectTubes->Begin(); it != objectTubes->End(); it++)
			{
				Model::TubePtr tube = *it;
				
				TubeShiftCostsPtr tubeShiftCosts = new TubeShiftCosts();
				(*collisionCostLookup)[tube] = tubeShiftCosts;

				for(Model::TubeCollection::Iterator it2 = objectTubes->Begin(); it2 != objectTubes->End(); it2++)
				{
					Model::TubePtr tube2 = *it2;
					if(tube != tube2)
					{
						ShiftCostsPtr shiftCosts = new ShiftCosts();
						(*tubeShiftCosts)[tube2] = shiftCosts;
							
						int maxShift = tube->GetFrameSpan().GetLength();
						for(int shift = 0; shift <= maxShift; shift += shiftStep)
						{
							double shiftCost = getTubesPairCollisionCost(tube, tube2, shift);
							shiftCosts->push_back(shiftCost);
						}
					}
				}
			}
		}

		void initTemporalConsistencyCostLookup(Model::TubeCollectionPtr objectTubes)
		{
			temporalConsistencyCostLookup = new TemporalConsistencyCostLookup();

			for(Model::TubeCollection::Iterator it = objectTubes->Begin(); it != objectTubes->End(); it++)
			{
				Model::TubePtr tube = *it;				
				
				for(Model::TubeCollection::Iterator it2 = objectTubes->Begin(); it2 != objectTubes->End(); it2++)
				{
					Model::TubePtr tube2 = *it2;					
					
					if(tube != tube2 && tube->GetId() <= tube2->GetId())
					{
						FrameSpan intersection = tube->GetFrameSpan().Intersect(tube2->GetFrameSpan());						
						if(!intersection.IsEmpty())
						{
							TubesPair pair = std::make_pair(tube, tube2);
							(*temporalConsistencyCostLookup)[pair] = getTubesPairTemporalConsistencyCost(tube, tube2);
						}						
					}
				}
			}
		}

		double temporalConsistencyCostFunctionOverlapping(double bestDistance)
		{
			return exp(-bestDistance/sigma_space);
		}

		double temporalConsistencyCostFunctionIndependent(int timeDistance)
		{
			return exp(-((double)timeDistance)/sigma_time);
		}

		double getTubesPairTemporalConsistencyCost(Model::TubePtr tube1, Model::TubePtr tube2)
		{
			FrameSpan span1 = tube1->GetFrameSpan();
			FrameSpan span2 = tube2->GetFrameSpan();
			
			FrameSpan intersection = span1.Intersect(span2);
			CV_Assert(!intersection.IsEmpty());
			
			size_t idx1 = intersection.From-span1.From;
			size_t idx2 = intersection.From-span2.From;
			size_t length = intersection.GetLength();
			size_t end1 = idx1 + length - 1;
			
			Model::TubeFrameCollectionPtr frames1 = tube1->GetFrames();
			Model::TubeFrameCollectionPtr frames2 = tube2->GetFrames();

			double bestDistance = DBL_MAX;

			for(idx1, idx2; idx1 <= end1; idx1++, idx2++)
			{
			#if 0
				cv::Point center1 = Helpers::Geometry::RectCenter((*frames1)[idx1]->GetBoundingBox());
				cv::Point center2 = Helpers::Geometry::RectCenter((*frames2)[idx2]->GetBoundingBox());
				
				double distance = Helpers::Geometry::PointsDistance(center1, center2);
			#else
				cv::Rect bbox1 = (*frames1)[idx1]->GetBoundingBox();
				cv::Rect bbox2 = (*frames2)[idx2]->GetBoundingBox();

				int l1 = bbox1.x;
				int l2 = bbox2.x;
				int r1 = l1+bbox1.width;
				int r2 = l2+bbox2.width;
				int t1 = bbox1.y;
				int t2 = bbox2.y;
				int b1 = t1+bbox1.height;
				int b2 = t2+bbox2.height;

				double distance = MIN(abs(l1-l2), MIN(abs(l1-r2), MIN(abs(r1-l2), MIN(abs(r1-r2), MIN(abs(t1-t2), MIN(abs(t1-b2), MIN(abs(b1-t2), abs(b1-b2))))))));
			#endif

				bestDistance = MIN(distance, bestDistance);
			}

			return (bestDistance < DBL_MAX)? temporalConsistencyCostFunctionOverlapping(bestDistance) : 0;
		}

		double getStateActivityCost(OptimizationStatePtr state)
		{
			double cost = 0;

			for(OptimizationState::iterator it = state->begin(); it != state->end(); it++)
			{
				TubeStatePtr tubeState = *it;
				cost += (!tubeState->mapped) * (*activityCostLookup)[tubeState->tube];
			}

			return cost;
		}

		double getTubesPairCollisionCostIncrement(TubeStatePtr tubeState1, TubeStatePtr tubeState2)
		{
			if(tubeState1->mapped && tubeState2->mapped && tubeState1 != tubeState2 && tubeState1->start <= tubeState2->start)
			{
				FrameSpan span1(tubeState1->start, tubeState1->start+tubeState1->tube->GetFrameSpan().GetLength() - 1);
				FrameSpan span2(tubeState2->start, tubeState2->start+tubeState2->tube->GetFrameSpan().GetLength() - 1);
				FrameSpan intersection = span1.Intersect(span2);
							
				if(!intersection.IsEmpty())
				{
					int shift = abs(span2.From - span1.From);
					size_t idx = shift / shiftStep;

					CV_Assert(idx >= 0 && idx < (*(*(*collisionCostLookup)[tubeState1->tube])[tubeState2->tube]).size());
					return (*(*(*collisionCostLookup)[tubeState1->tube])[tubeState2->tube])[idx];
				}
			}

			return 0;
		}

		double getStateCollisionCost(OptimizationStatePtr state)
		{
			double cost = 0;

			for(OptimizationState::iterator it1 = state->begin(); it1 != state->end(); it1++)
			{
				TubeStatePtr tubeState1 = *it1;
				if(tubeState1->mapped)
				{				
					for(OptimizationState::iterator it2 = state->begin(); it2 != state->end(); it2++)
					{
						TubeStatePtr tubeState2 = *it2;
						cost += getTubesPairCollisionCostIncrement(tubeState1, tubeState2);
					}
				}
			}

			return cost;
		}

		double getTubesPairTemporalConsistencyCostIncrement(TubeStatePtr tubeState1, TubeStatePtr tubeState2)
		{			
			Model::TubePtr tube1 = tubeState1->tube;
			Model::TubePtr tube2 = tubeState2->tube;

			FrameSpan span1 = tube1->GetFrameSpan();
			FrameSpan span2 = tube2->GetFrameSpan();

			if(tubeState2->mapped && tubeState2->mapped && tube1 != tube2 && tubeState1->tube->GetId() <= tubeState2->tube->GetId())
			{
				if(tubeState1->start - tubeState2->start != span1.From - span2.From)
				{
					FrameSpan intersection = span1.Intersect(span2);
					if(intersection.IsEmpty())
					{
						if(tubeState2->start <= tubeState1->start)
							return temporalConsistencyCostFunctionIndependent(tubeState1->start - (tubeState2->start+span2.GetLength()));
						else
							return temporalConsistencyCostFunctionIndependent(tubeState2->start - (tubeState1->start+span1.GetLength()));
					}
					else
					{
						TubesPair pair = (tube1->GetId() <= tube2->GetId())? std::make_pair(tube1, tube2) : std::make_pair(tube2, tube1);
						return (*temporalConsistencyCostLookup)[pair];
					}
				}
			}

			return 0;
		}

		double getStateTemporalConsistencyCost(OptimizationStatePtr state)
		{
			double cost = 0;
			
			for(size_t i1 = 0; i1 < state->size(); i1++)
			{
				TubeStatePtr tubeState1 = (*state)[i1];
				for(size_t i2 = 0; i2 < state->size(); i2++)
				{
					TubeStatePtr tubeState2 = (*state)[i2];
					cost += getTubesPairTemporalConsistencyCostIncrement(tubeState1, tubeState2);
				}
			}
			
			return cost;
		}

		double weightedAverage(double activityCost, double collisionCost, double temporalConsistencyCost)
		{
			return activityCost + alpha*collisionCost + beta*temporalConsistencyCost;
		}

	public:
		AtcEnergyFunctionEstimator(int shiftStep, double alpha, double beta, double sigma_time, double sigma_space) : EnergyFunctionEstimator()
		{
			this->activityCostLookup = NULL;
			this->collisionCostLookup = NULL;
			this->temporalConsistencyCostLookup = NULL;

			this->shiftStep = shiftStep;
			this->alpha = alpha;
			this->beta = beta;
			this->sigma_time = sigma_time;
			this->sigma_space = sigma_space;
		}

		virtual void Init(Model::TubeCollectionPtr objectTubes)
		{
			initActivityCostLookup(objectTubes);
			initCollisionCostLookup(objectTubes);
			initTemporalConsistencyCostLookup(objectTubes);
		}

		virtual double Compute(OptimizationStatePtr state)
		{
			double activityCost = getStateActivityCost(state);
			double collisionCost = getStateCollisionCost(state);
			double temporalConsistencyCost = getStateTemporalConsistencyCost(state);
		
			std::cout << activityCost << " + " << alpha*collisionCost << " + " << beta*temporalConsistencyCost << "\n";
			std::cout << "    " << "(activityCost=" << activityCost << ")\n";
			std::cout << "--  " << "(collisionCost=" << collisionCost << ")\n";
			std::cout << "    " << "(temporalConsistencyCost=" << temporalConsistencyCost << ")\n";

			return weightedAverage(activityCost, collisionCost, temporalConsistencyCost);
		}

		virtual double ComputeIncrementForInclusionChange(OptimizationStatePtr state, size_t idx)
		{
			TubeStatePtr tubeState = (*state)[idx];
			Model::TubePtr tube = tubeState->tube;

			double activityCostIncrement = (tubeState->mapped)? -(*activityCostLookup)[tube] : (*activityCostLookup)[tube];			
			double collisionCostIncrement = 0;
			double temporalConsistencyCostIncrement = 0;
			
			tubeState->mapped = !tubeState->mapped;
			for(size_t i2 = 0; i2 < state->size(); i2++)
			{
				TubeStatePtr tubeState2 = (*state)[i2];
				
				temporalConsistencyCostIncrement -= getTubesPairTemporalConsistencyCostIncrement(tubeState, tubeState2);
				temporalConsistencyCostIncrement -= getTubesPairTemporalConsistencyCostIncrement(tubeState2, tubeState);
				
				collisionCostIncrement -= getTubesPairCollisionCostIncrement(tubeState, tubeState2);
				collisionCostIncrement -= getTubesPairCollisionCostIncrement(tubeState2, tubeState);
			}

			tubeState->mapped = !tubeState->mapped;
			for(size_t i2 = 0; i2 < state->size(); i2++)
			{
				TubeStatePtr tubeState2 = (*state)[i2];
				
				temporalConsistencyCostIncrement += getTubesPairTemporalConsistencyCostIncrement(tubeState, tubeState2);
				temporalConsistencyCostIncrement += getTubesPairTemporalConsistencyCostIncrement(tubeState2, tubeState);
				
				collisionCostIncrement += getTubesPairCollisionCostIncrement(tubeState, tubeState2);
				collisionCostIncrement += getTubesPairCollisionCostIncrement(tubeState2, tubeState);
			}

			return weightedAverage(activityCostIncrement, collisionCostIncrement, temporalConsistencyCostIncrement);
		}

		virtual double ComputeIncrementForShiftChange(OptimizationStatePtr state, size_t idx, int oldValue)
		{
			TubeStatePtr tubeState = (*state)[idx];
			Model::TubePtr tube = tubeState->tube;
			int newValue = tubeState->start;
			
			double collisionCostIncrement = 0;
			double temporalConsistencyCostIncrement = 0;
			
			tubeState->start = oldValue;
			for(size_t i2 = 0; i2 < state->size(); i2++)
			{
				TubeStatePtr tubeState2 = (*state)[i2];
				temporalConsistencyCostIncrement -= getTubesPairTemporalConsistencyCostIncrement(tubeState, tubeState2);
				temporalConsistencyCostIncrement -= getTubesPairTemporalConsistencyCostIncrement(tubeState2, tubeState);

				collisionCostIncrement -= getTubesPairCollisionCostIncrement(tubeState, tubeState2);
				collisionCostIncrement -= getTubesPairCollisionCostIncrement(tubeState2, tubeState);
			}

			tubeState->start = newValue;
			for(size_t i2 = 0; i2 < state->size(); i2++)
			{
				TubeStatePtr tubeState2 = (*state)[i2];
				temporalConsistencyCostIncrement += getTubesPairTemporalConsistencyCostIncrement(tubeState, tubeState2);
				temporalConsistencyCostIncrement += getTubesPairTemporalConsistencyCostIncrement(tubeState2, tubeState);

				collisionCostIncrement += getTubesPairCollisionCostIncrement(tubeState, tubeState2);
				collisionCostIncrement += getTubesPairCollisionCostIncrement(tubeState2, tubeState);
			}

			return weightedAverage(0, collisionCostIncrement, temporalConsistencyCostIncrement);
		}

	};

	int shiftStep;

	static OptimizationStatePtr createOptimizationState(Model::TubeCollectionPtr tubes)
	{
		CV_Assert(tubes != NULL);

		OptimizationStatePtr state = new OptimizationState();

		for(Model::TubeCollection::Iterator it = tubes->Begin(); it != tubes->End(); it++)
		{
			Model::TubePtr tube = *it;			
			state->push_back(new TubeState(tube));
		}

		return state;
	}

	static Model::Scenario::TubeMappingCollectionPtr createMapping(OptimizationStatePtr state)
	{
		CV_Assert(state != NULL);
		
		Model::Scenario::TubeMappingCollectionPtr mapping = new Model::Scenario::TubeMappingCollection();

		for(OptimizationState::iterator it = state->begin(); it != state->end(); it++)
		{
			if((*it)->mapped)
			{
				Model::TubePtr tube = (*it)->tube;
				FramePosition from = (*it)->start;
				FramePosition to = from + tube->GetFrameSpan().To - tube->GetFrameSpan().From;
			
				mapping->push_back(new Model::Scenario::TubeMapping(tube, from, to));
			}
		}

		return mapping;
	}

public:
	BaseSynopsisScenarioSolver(Model::TubeCollectionPtr objectTubes = NULL) : IScenarioSolver(objectTubes)
	{
		SetShiftStep(10);
	}

	virtual ~BaseSynopsisScenarioSolver()
	{
	}

	void SetShiftStep(int shiftStep)
	{
		CV_Assert(shiftStep > 0);
		this->shiftStep = shiftStep;
	}
};


}}}

#endif // BASESYNOPSISSCENARIOSOLVER_H
