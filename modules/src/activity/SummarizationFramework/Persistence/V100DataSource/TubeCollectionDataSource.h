#pragma once
#ifndef V100DATASOURCE_TUBECOLLECTIONDATASOURCE_H
#define XXXXXXXXXXXXXX

#include "../../Model/TubeCollection.h"
#include "FileBasedDataSource.h"
#include "TubeDataSource.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace V100DataSource
{


class TubeCollectionDataSource : public FileBasedDataSource<Model::TubeCollection>
{
protected:
	static const char *NODE_ROOT;
	static const char *NODE_HEADER;
	static const char *NODE_DATA;
	static const char *NODE_DATA_TUBE;

	Model::TubeCollectionPtr internalLoad(cv::FileNode node)
	{
		cv::FileNode rootNode = CheckNode(node[NODE_ROOT]);
		cv::FileNode headerNode = CheckNode(rootNode[NODE_HEADER]);
		cv::FileNode dataNode = CheckNode(rootNode[NODE_DATA]);
		
		Model::TubeCollectionPtr collection = new Model::TubeCollection();
		
		for(cv::FileNodeIterator it = dataNode.begin(); it != dataNode.end(); it++)
		{
			Model::Tube::DataSourcePtr dataSource = new TubeDataSource(*it, params);
			Model::TubePtr tube = new Model::Tube(dataSource);
			collection->Insert(tube);
			tube->Load();
		}

		return collection;
	}

	void internalSave(cv::FileStorage *storage, Model::TubeCollection *entity)
	{
		(*storage) << NODE_ROOT << "{";

		(*storage) << NODE_HEADER << "[" << "]";

		(*storage) << NODE_DATA << "[";
			for(Model::TubeCollection::Iterator it = entity->Begin(); it != entity->End(); it++)
			{
				std::string filename = params.GetTubeDataXmlFilePath((*it)->GetId());

				Model::TubePtr tube = *it;
				tube->Load();

				if(tube->GetFrames() != NULL)
				{
					Model::Tube::DataSourcePtr oldDataSource = tube->GetDataSource();
					tube->SetDataSource(new TubeDataSource(filename, params));
					tube->Save();
					tube->SetDataSource(oldDataSource);
				}

				(*storage) << filename;
			}
		(*storage) << "]"; // NODE_DATA

		(*storage) << "}"; // NODE_ROOT
	}

public:
	TubeCollectionDataSource(std::string filename, PathParams params) : FileBasedDataSource(filename, params)
	{
	}
};

typedef cv::Ptr<TubeCollectionDataSource> TubeCollectionDataSourcePtr;

#ifndef QT
const char *TubeCollectionDataSource::NODE_ROOT = "tubes";
const char *TubeCollectionDataSource::NODE_HEADER = "header";
const char *TubeCollectionDataSource::NODE_DATA = "data";
const char *TubeCollectionDataSource::NODE_DATA_TUBE = "tube";
#endif


}}}

#endif // V100DATASOURCE_TUBECOLLECTIONDATASOURCE_H
