#pragma once
#ifndef FILEDATASOURCE_TUBECOLLECTIONFILEDATASOURCE_H
#define FILEDATASOURCE_TUBECOLLECTIONFILEDATASOURCE_H

#include "TubeFileDataSource.h"
#include "../../Model/TubeCollection.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace FileDataSource
{


class TubeCollectionFileDataSource : public BaseFileDataSource<Model::TubeCollection>
{
protected:
	static const char *NODE_TUBES;

	virtual Model::TubeCollectionPtr internalLoad(cv::FileNode node)
	{
		cv::FileNode tubesNode = node[NODE_TUBES];		
		Model::TubeCollectionPtr collection = new Model::TubeCollection();

		for(cv::FileNodeIterator it = tubesNode.begin(); it != tubesNode.end(); it++)
		{
			Model::TubePtr tube = new Model::Tube(new TubeFileDataSource(*it));
			tube->Load();
			tube->SetDataSource(NULL);

			collection->Insert(tube);
		}

		return collection;
	}

	virtual void internalSave(cv::FileStorage *storage, Model::TubeCollection *entity)
	{
		entity->Load();

		(*storage) << NODE_TUBES << "[";
			for(Model::TubeCollection::Iterator it = entity->Begin(); it != entity->End(); it++)
			{
				(*storage) << "{";

				Model::TubePtr tube = *it;
				tube->Load();

				Model::Tube::DataSourcePtr oldDataSource = tube->GetDataSource();
				tube->SetDataSource(new TubeFileDataSource(storage));
				tube->Save();
				tube->SetDataSource(oldDataSource);

				(*storage) << "}";
			}
		(*storage) << "]";
	}

public:
	TubeCollectionFileDataSource(cv::FileNode node) : BaseFileDataSource(node)
	{
	}

	TubeCollectionFileDataSource(cv::FileStorage *storage) : BaseFileDataSource(storage)
	{
	}
};

typedef cv::Ptr<TubeCollectionFileDataSource> TubeCollectionFileDataSourcePtr;

#ifndef QT
const char *TubeCollectionFileDataSource::NODE_TUBES = "tubes";
#endif


}}}

#endif // FILEDATASOURCE_TUBECOLLECTIONFILEDATASOURCE_H
