#pragma once
#ifndef TUBEFRAME_H
#define TUBEFRAME_H

#include "../CommonTypes.h"
#include "../Persistence/IDataSource.h"
#include "../Persistence/IPersistable.h"
#include "TubeFrameData.h"

namespace SummarizationFramework
{
namespace Model
{


class TubeFrame;
typedef cv::Ptr<TubeFrame> TubeFramePtr;
typedef std::vector<TubeFramePtr> TubeFrameCollection;
typedef cv::Ptr<TubeFrameCollection> TubeFrameCollectionPtr;


class TubeFrame : public Persistence::IPersistable<TubeFrame>
{
private:
	TubeFrameDataPtr data;
	cv::Rect boundingBox;
	FramePosition framePosition;

public:
	TubeFrame(DataSourcePtr dataSource) : Persistence::IPersistable<TubeFrame>(dataSource)
	{
        this->data.release();
		this->boundingBox = cv::Rect();
		this->framePosition = FRAMESPAN_EMPTY;
	}

	TubeFrame(TubeFrameDataPtr data, cv::Rect boundingBox, FramePosition framePosition) : Persistence::IPersistable<TubeFrame>()
	{
		this->data = data;
		this->boundingBox = boundingBox;
		this->framePosition = framePosition;
		this->isLoaded = true;
	}

	TubeFrameDataPtr GetData()
	{
		lazyLoad();
		return data;
	}

	cv::Rect GetBoundingBox()
	{
		lazyLoad();
		return boundingBox;
	}

	FramePosition GetFramePosition()
	{
		lazyLoad();
		return framePosition;
	}

	virtual void Load()
	{
		if(dataSource != NULL && !isLoaded)
		{
			TubeFramePtr entity = dataSource->Load();
			
			data = entity->data;
			boundingBox = entity->boundingBox;
			framePosition = entity->framePosition;

			isLoaded = true;
		}
	}

	virtual void Save()
	{
		if(dataSource != NULL)
		{
			dataSource->Save(this);
		}
	}
};


}}

#endif // TUBEFRAME_H
