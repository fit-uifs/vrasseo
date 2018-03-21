#pragma once
#ifndef IPERSISTABLE_H
#define IPERSISTABLE_H

#include "../CommonTypes.h"
#include "IDataSource.h"

namespace SummarizationFramework
{
namespace Persistence
{


template<typename T>
class IPersistable
{
public:
	typedef cv::Ptr<T> TPtr;
	typedef IDataSource<T> DataSource;
	typedef cv::Ptr<DataSource> DataSourcePtr;

protected:
	DataSourcePtr dataSource;
	bool isLoaded;

	void lazyLoad()
	{
		if(!isLoaded)
		{
			Load();
		}
	}

public:
	IPersistable()
	{
		this->isLoaded = false;
	}

	IPersistable(DataSourcePtr dataSource)
	{
		this->dataSource = dataSource;
		this->isLoaded = false;
	}

	DataSourcePtr GetDataSource()
	{
		return dataSource;
	}

	void SetDataSource(DataSourcePtr dataSource, bool clearLoadedFlag = false)
	{
		this->dataSource = dataSource;

		if(clearLoadedFlag)
		{
			isLoaded = false;
		}
	}

	bool IsLoaded()
	{
		return isLoaded;
	}

	virtual TPtr LoadAside()
	{
        return dataSource ? dataSource->Load() : TPtr();
	}

	virtual void Load() = 0;	
	virtual void Save() = 0;
};


}}

#endif // IPERSISTABLE_H
