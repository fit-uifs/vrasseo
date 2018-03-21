#pragma once
#ifndef TUBE_H
#define TUBE_H

#include "../CommonTypes.h"
#include "../Persistence/IDataSource.h"
#include "../Persistence/IPersistable.h"
#include "TubeFrame.h"

namespace SummarizationFramework
{
namespace Model
{


class Tube;
typedef cv::Ptr<Tube> TubePtr;

class Tube : public Persistence::IPersistable<Tube>
{
private:
	TubeFrameCollectionPtr frames;
	FrameSpan frameSpan;
	std::string fileName;
	int id;

protected:
	void updateFrameSpan()
	{
		frameSpan = FrameSpan();

		for(TubeFrameCollection::iterator it = frames->begin(); it != frames->end(); it++)
		{
            int currentframePosition = (*it)->GetFramePosition();

			if(frameSpan.IsEmpty())
			{
				frameSpan.From = currentframePosition;
				frameSpan.To = currentframePosition;
			}
			else
			{
				if(currentframePosition > frameSpan.To)
				{
					frameSpan.To = currentframePosition;
				}

				if(currentframePosition < frameSpan.From)
				{
					frameSpan.From = currentframePosition;
				}
			}
		}
	}

	static int generateId()
	{
		static int nextId = 0;
		return ++nextId;
	}

public:
	Tube(DataSourcePtr dataSource) : Persistence::IPersistable<Tube>(dataSource)
	{
		this->frames = new TubeFrameCollection();
		this->frameSpan = FrameSpan();
	}

	Tube(TubeFrameCollectionPtr frames, FrameSpan frameSpan, std::string fileName, int id = -1) : Persistence::IPersistable<Tube>()
	{
		this->frames = frames;
		this->frameSpan = frameSpan;
		this->fileName = fileName;
		this->isLoaded = true;
		this->id = (id != -1)? id : generateId();

		if(frameSpan.IsEmpty() && frames != NULL)
		{
			updateFrameSpan();
		}
	}

	TubeFrameCollectionPtr GetFrames()
	{
		lazyLoad();
		return frames;
	}

	FrameSpan GetFrameSpan()
	{
		lazyLoad();
		return frameSpan;
	}

	std::string GetFileName()
	{
		lazyLoad();
		return fileName;
	}

	int GetId()
	{
		lazyLoad();
		return id;
	}

	virtual void Load()
	{
		if(dataSource != NULL && !isLoaded)
		{
			TubePtr entity = dataSource->Load();

			frames = entity->frames;
			frameSpan = entity->frameSpan;
			fileName = entity->fileName;
			id = entity->id;

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

#endif // TUBE_H
