#pragma once
#ifndef IBACKGROUNDSOLVER_H
#define IBACKGROUNDSOLVER_H

#include "../../Model/IVideoSource.h"
#include "../../Model/Scenario.h"

namespace SummarizationFramework
{
namespace Optimization
{
namespace BackgroundSolver
{


class IBackgroundSolver
{
public:
	virtual ~IBackgroundSolver()
	{
	}
	virtual void Execute(Model::ScenarioPtr scenario, Model::IVideoSourcePtr originalVideo) = 0;
	virtual Model::IVideoSourcePtr GetBackgroundVideo() = 0;
};

typedef cv::Ptr<IBackgroundSolver> IBackgroundSolverPtr;


}}}

#endif // IBACKGROUNDSOLVER_H
