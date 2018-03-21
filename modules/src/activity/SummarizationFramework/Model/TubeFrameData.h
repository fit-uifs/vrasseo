#pragma once
#ifndef TUBEFRAMEDATA_H
#define TUBEFRAMEDATA_H

#include "../CommonTypes.h"
#include "../Persistence/IDataSource.h"
#include "../Persistence/IPersistable.h"

namespace SummarizationFramework
{
namespace Model
{


class TubeFrameData;
typedef cv::Ptr<TubeFrameData> TubeFrameDataPtr;


class TubeFrameData : public Persistence::IPersistable<TubeFrameData>
{
private:
	FrameMask mask;

public:
	TubeFrameData(DataSourcePtr dataSource) : Persistence::IPersistable<TubeFrameData>(dataSource)
	{
		this->mask = FrameMask();
	}

	TubeFrameData(FrameMask mask) : Persistence::IPersistable<TubeFrameData>()
	{
		this->mask = mask;
		this->isLoaded = true;
	}

	FrameMask GetMask()
	{
		lazyLoad();
		return mask;
	}

	virtual void Load()
	{
		if(dataSource != NULL && !isLoaded)
		{
			TubeFrameDataPtr entity = dataSource->Load();

			mask = entity->mask;

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

#endif // TUBEFRAMEDATA_H
