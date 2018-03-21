#pragma once
#ifndef IRENDERER_H
#define IRENDERER_H

#include "../CommonTypes.h"
#include "../Model/Scenario.h"

namespace SummarizationFramework
{
namespace Presentation
{


class IRenderer
{
public:
	class Params
	{
	public:
		double fps;
		int fourcc;

		Params(double fps = 25, int fourcc = -1)
		{
			this->fps = fps;
			this->fourcc = fourcc;
		}
	};
	
	class FrameCreatedCallback
	{
	public:
	    virtual void OnFrameCreated(Frame & frame, int no, int length) = 0;
	};
	
	typedef cv::Ptr<FrameCreatedCallback> FrameCreatedCallbackPtr;

protected:
	Params params;
	FrameCreatedCallbackPtr frameCreatedCallback;
	
	virtual void internalRender(Model::ScenarioPtr scenario, Model::IVideoSourcePtr background, std::string filename) = 0;
	void onFrameCreated(Frame & frame, int no, int length)
	{
	    if(frameCreatedCallback != NULL)
		{
			frameCreatedCallback->OnFrameCreated(frame, no, length);
		}
	}

public:
	IRenderer(Params params = Params())
	{
		SetParams(params);
		SetFrameCreatedCallback(NULL);
	}

	virtual ~IRenderer()
	{
	}

	virtual void Render(Model::ScenarioPtr scenario, Model::IVideoSourcePtr background, std::string filename)
	{
		scenario->Load();
		internalRender(scenario, background, filename);
	}

	void SetParams(Params params)
	{
		this->params = params;
	}
	
	void SetFrameCreatedCallback(FrameCreatedCallbackPtr frameCreatedCallback)
	{
	    this->frameCreatedCallback = frameCreatedCallback;
	}
};


typedef cv::Ptr<IRenderer> IRendererPtr;


}}

#endif // IRENDERER_H
