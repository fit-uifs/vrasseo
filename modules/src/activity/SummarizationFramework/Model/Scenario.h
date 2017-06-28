#pragma once
#ifndef SCENARIO_H
#define SCENARIO_H

#include "../CommonTypes.h"
#include "../Persistence/IDataSource.h"
#include "../Persistence/IPersistable.h"
#include "Tube.h"

namespace SummarizationFramework
{
namespace Model
{


class Scenario;
typedef cv::Ptr<Scenario> ScenarioPtr;

class Scenario : public Persistence::IPersistable<Scenario>
{
public:
	class TubeMapping;
	
	typedef cv::Ptr<TubeMapping> TubeMappingPtr;
	typedef std::vector<TubeMappingPtr> TubeMappingCollection;
	typedef cv::Ptr<TubeMappingCollection> TubeMappingCollectionPtr;

private:
	TubeMappingCollectionPtr mapping;
	int length;

public:
	Scenario() : Persistence::IPersistable<Scenario>()
	{
		this->mapping = new TubeMappingCollection();
		this->isLoaded = true;
	}

	Scenario(TubeMappingCollectionPtr mapping, int length) : Persistence::IPersistable<Scenario>()
	{
		this->mapping = mapping;
		this->length = length;
		this->isLoaded = true;
	}
	
	Scenario(DataSourcePtr dataSource) : Persistence::IPersistable<Scenario>(dataSource)
	{
		this->mapping = new TubeMappingCollection();
	}

	TubeMappingCollectionPtr GetMapping()
	{
		lazyLoad();
		return mapping;
	}

	int GetLength()
	{
		lazyLoad();
		return length;
	}

	virtual void Load()
	{
		if(dataSource != NULL && !isLoaded)
		{
			ScenarioPtr entity = dataSource->Load();
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

class Scenario::TubeMapping
{
private:
	TubePtr tube;
	FramePosition from;
	FramePosition to;

public:
	TubeMapping(TubePtr tube, FramePosition from, FramePosition to)
	{
		this->tube = tube;
		this->from = from;
		this->to = to;
	}

	TubePtr GetTube() const
	{
		return tube;
	}

	FramePosition GetFirstFramePosition() const
	{
		return from;
	}

	FramePosition GetLastFramePosition() const
	{
		return to;
	}

	FrameSpan GetFrameSpan() const
	{
		return FrameSpan(from, to);
	}

	int GetLength()
	{
		return to - from + 1;
	}
};


}}

#endif // SCENARIO_H
