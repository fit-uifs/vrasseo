#pragma once
#ifndef STATICRENDERER_H
#define STATICRENDERER_H

#include "SynopsisRenderer.h"
#include "../ImageProcessing/FrameFilters/FullFrameBlender.h"
#include "../Exceptions.h"

namespace SummarizationFramework
{
namespace Presentation
{


class StaticRenderer : public SynopsisRenderer
{
public:
	StaticRenderer(Params params = Params()) : SynopsisRenderer(params)
	{
		((SynopsisRenderer *)this)->SetFrameBlender(new ImageProcessing::FrameFilters::FullFrameBlender());
	}

	virtual ~StaticRenderer()
	{
	}

	void SetFrameBlender(ImageProcessing::FrameFilters::FrameBlenderPtr blender)
	{
		throw new NotSupportedException("StaticRenderer's FrameBlender cannot be changed.");
	}
};

typedef cv::Ptr<StaticRenderer> StaticRendererPtr;


}}

#endif // STATICRENDERER_H
