#pragma once
#ifndef LINEARMOGBACKGROUNDSOLVER_H
#define LINEARMOGBACKGROUNDSOLVER_H

#include "IBackgroundSolver.h"
#include "../../ImageProcessing/VideoFilters/MogBackgroundExtractorVideoFilter.h"
#include "../../ImageProcessing/VideoFilters/TimelapseVideoFilter.h"

namespace SummarizationFramework
{
namespace Optimization
{
namespace BackgroundSolver
{


class LinearMogBackgroundSolver : public IBackgroundSolver
{
private:
	Model::ScenarioPtr scenario;
	Model::IVideoSourcePtr originalVideo;
	
	bool sparse;
	double increaseFactor;

public:
	LinearMogBackgroundSolver(bool sparse = true, double increaseFactor = 1.0) : IBackgroundSolver()
	{
		SetSparseMode(sparse);
		SetIncreaseFactor(increaseFactor);
	}

	virtual ~LinearMogBackgroundSolver()
	{
	}

	void SetIncreaseFactor(double increaseFactor)
	{
		CV_Assert(increaseFactor >= 1.0);
		this->increaseFactor = increaseFactor;
	}

	void SetSparseMode(bool sparse)
	{
		this->sparse = sparse;
	}

	virtual void Execute(Model::ScenarioPtr scenario, Model::IVideoSourcePtr originalVideo)
	{
		this->scenario = scenario;
		this->originalVideo = originalVideo;
	}

	virtual Model::IVideoSourcePtr GetBackgroundVideo()
	{
		CV_Assert(originalVideo->GetLength() >= scenario->GetLength());

		Model::IVideoSourcePtr firstStageVideo = originalVideo;

		if(sparse)
		{
			double maxSubSamplingFactor = (double)originalVideo->GetLength() / scenario->GetLength();		
			double firstStageSubSamplingFactor = maxSubSamplingFactor/increaseFactor;
			int firstStageLength = MIN(scenario->GetLength(), (int)ceil(originalVideo->GetLength()/firstStageSubSamplingFactor));

			firstStageVideo = new ImageProcessing::VideoFilters::TimelapseVideoFilter(originalVideo, firstStageLength);
		}

		Model::IVideoSourcePtr videoFiltered = new ImageProcessing::VideoFilters::MogBackgroundExtractorVideoFilter(firstStageVideo);
		Model::IVideoSourcePtr videoBackground = new ImageProcessing::VideoFilters::TimelapseVideoFilter(videoFiltered, scenario->GetLength());

		return videoBackground;
	}
};

typedef cv::Ptr<LinearMogBackgroundSolver> LinearMogBackgroundSolverPtr;


}}}

#endif // LINEARMOGBACKGROUNDSOLVER_H
