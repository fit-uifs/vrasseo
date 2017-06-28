#pragma once
#ifndef V100DATASOURCE_FILEBASEDDATASOURCE_H
#define V100DATASOURCE_FILEBASEDDATASOURCE_H

#include "../IDataSource.h"
#include "PathParams.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace V100DataSource
{


template<typename T>
class FileBasedDataSource : public IDataSource<T>
{
protected:
	PathParams params;
	std::string filename;

	virtual TPtr internalLoad(cv::FileNode node) = 0;
	virtual void internalSave(cv::FileStorage *storage, T *entity) = 0;

public:
	FileBasedDataSource(std::string filename, PathParams params) : IDataSource()
	{
		this->filename = filename;
		this->params = params;
	}

	virtual TPtr Load()
	{
		cv::FileStorage fileStorage(filename, cv::FileStorage::READ);
		return internalLoad(fileStorage.root());
	}

	virtual void Save(T *entity)
	{
		cv::FileStorage fileStorage(filename, cv::FileStorage::WRITE);
		
		entity->Load();
		internalSave(&fileStorage, entity);
		
		fileStorage.release();
	}

	static cv::FileNode CheckNode(cv::FileNode & node)
	{
		return XmlBasedDataSource<T>::CheckNode(node);
	}
};


}}}

#endif // V100DATASOURCE_FILEBASEDDATASOURCE_H
