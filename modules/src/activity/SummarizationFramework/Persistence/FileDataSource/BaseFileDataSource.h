#pragma once
#ifndef FILEDATASOURCE_BASEFILEDATASOURCE_H
#define FILEDATASOURCE_BASEFILEDATASOURCE_H

#include "../IDataSource.h"
#include "../../Model/TubeFrame.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace FileDataSource
{


template<typename T>
class BaseFileDataSource : public IDataSource<T>
{
private:
	cv::FileNode node;
	cv::FileStorage *storage;
	bool readMode;

protected:	
	cv::FileNode checkedNode(cv::FileNode & node)
	{
		if(node.empty())
		{
			throw 0; // TODO add meaningful exception
		}

		return node;
	}

	virtual TPtr internalLoad(cv::FileNode node) = 0;
	virtual void internalSave(cv::FileStorage *storage, T *entity) = 0;

public:
	BaseFileDataSource(cv::FileNode node)
	{
		this->node = node;
		this->readMode = true;
	}

	BaseFileDataSource(cv::FileStorage *storage)
	{
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

		internalSave(storage, entity);
	}
};


}}}

#endif // FILEDATASOURCE_BASEFILEDATASOURCE_H
