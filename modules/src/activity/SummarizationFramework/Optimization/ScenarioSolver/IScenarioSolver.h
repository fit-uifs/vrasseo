#pragma once
#ifndef ISCENARIOSOLVER_H
#define ISCENARIOSOLVER_H

#include "../../CommonTypes.h"
#include "../../Model/Scenario.h"
#include "../../Model/TubeCollection.h"
#include "../../Persistence/IDataSource.h"

namespace SummarizationFramework
{
namespace Optimization
{
namespace ScenarioSolver
{

class IScenarioSolver
{
public:
	class DataSourceAdapter : public Model::Scenario::DataSource
	{
	private:
		IScenarioSolver *solver;

	public:
		DataSourceAdapter(IScenarioSolver *solver)
		{
			this->solver = solver;
		}

		virtual Model::ScenarioPtr Load()
		{
			solver->Execute();
			return solver->GetScenario();
		}

		virtual void Save(Model::Scenario *entity)
		{
			throw NotSupportedException("IScenarioSolver::DataSourceAdapter does not support save.");
		}
	};

private:
	Model::ScenarioPtr scenario;
	Model::TubeCollectionPtr objectTubes;

protected:
	virtual Model::ScenarioPtr internalExecute(Model::TubeCollectionPtr objectTubes) = 0;

public:
	IScenarioSolver(Model::TubeCollectionPtr objectTubes = NULL)
	{
		this->objectTubes = objectTubes;
		this->scenario = NULL;
	}

	virtual ~IScenarioSolver()
	{
	}

	void Execute()
	{
		CV_Assert(objectTubes != NULL);
		scenario = internalExecute(objectTubes);
	}

	void Execute(Model::TubeCollectionPtr objectTubes)
	{
		CV_Assert(objectTubes != NULL);
		this->objectTubes = objectTubes;
		scenario = internalExecute(objectTubes);
	}

	Model::ScenarioPtr GetScenario()
	{
		return scenario;
	}

	cv::Ptr<Persistence::IDataSource<Model::Scenario> > AsDataSource()
	{
		CV_Assert(objectTubes != NULL);
		return new DataSourceAdapter(this);
	}
};

typedef cv::Ptr<IScenarioSolver> IScenarioSolverPtr;


}}}

#endif // ISCENARIOSOLVER_H
