#pragma once
#ifndef V100DATASOURCE_XMLBASEDDATASOURCE_H
#define V100DATASOURCE_XMLBASEDDATASOURCE_H

#include "../IDataSource.h"
#include "PathParams.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace V100DataSource
{


template<typename T>
class XmlBasedDataSource : public IDataSource<T>
{
private:
	cv::FileNode node;
	cv::FileStorage *storage;
	bool readMode;

protected:	
	PathParams params;

	virtual TPtr internalLoad(cv::FileNode node) = 0;
	virtual void internalSave(cv::FileStorage *storage, T *entity) = 0;

public:
	XmlBasedDataSource(PathParams params, cv::FileNode node)
	{
		this->params = params;
		this->node = node;
		this->readMode = true;
	}

	XmlBasedDataSource(PathParams params, cv::FileStorage *storage)
	{
		this->params = params;
		this->storage = storage;
		this->readMode = false;
	}

	virtual TPtr Load()
	{
		if(!readMode)
		{
			throw 0; // TODO add meaningful exception
		}

		return internalLoad(node);
	}

	virtual void Save(T *entity)
	{
		if(readMode)
		{
			throw 0; // TODO add meaningful exception
		}

		entity->Load();
		internalSave(storage, entity);
	}

	bool IsReadMode()
	{
		return readMode;
	}

	static cv::FileNode CheckNode(cv::FileNode & node)
	{
		if(node.empty())
		{
			throw 0; // TODO add meaningful exception
		}

		return node;
	}
};


}}}

#endif // V100DATASOURCE_XMLBASEDDATASOURCE_H
