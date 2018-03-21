#pragma once
#ifndef FILEDATASOURCE_TUBEFRAMEFILEDATASOURCE_H
#define FILEDATASOURCE_TUBEFRAMEFILEDATASOURCE_H

#include "TubeFrameDataFileDataSource.h"
#include "../../Model/TubeFrame.h"
#include <iostream>

namespace SummarizationFramework
{
namespace Persistence
{
namespace FileDataSource
{


class TubeFrameFileDataSource : public BaseFileDataSource<Model::TubeFrame>
{
protected:
	static const char *NODE_DATA;
	static const char *NODE_BOUNDING_BOX;
	static const char *NODE_FRAME_POSITION;

	virtual Model::TubeFramePtr internalLoad(cv::FileNode node)
	{
		cv::FileNode dataNode = checkedNode(node[NODE_DATA]);
		cv::FileNode boundingBoxNode = checkedNode(node[NODE_BOUNDING_BOX]);
		cv::FileNode framePositionNode = checkedNode(node[NODE_FRAME_POSITION]);

		Model::TubeFrameDataPtr data = new Model::TubeFrameData(new TubeFrameDataFileDataSource(dataNode));
		data->Load();
		data->SetDataSource(NULL);

		std::vector<int> boundingBoxVector;
		boundingBoxNode >> boundingBoxVector;
		cv::Rect boundingBox(boundingBoxVector[0], boundingBoxVector[1], boundingBoxVector[2], boundingBoxVector[3]);

		FramePosition framePosition;
		framePositionNode >> framePosition;

		return new Model::TubeFrame(data, boundingBox, framePosition);

	}

	virtual void internalSave(cv::FileStorage *storage, Model::TubeFrame *entity)
	{
		entity->Load();

		(*storage) << NODE_FRAME_POSITION << entity->GetFramePosition();
		(*storage) << NODE_BOUNDING_BOX << entity->GetBoundingBox();

		(*storage) << NODE_DATA << "{";
			Model::TubeFrameDataPtr data = entity->GetData();
			Model::TubeFrameData::DataSourcePtr oldDataSource = data->GetDataSource();
			data->SetDataSource(new TubeFrameDataFileDataSource(storage));
			data->Save();
			data->SetDataSource(oldDataSource);		
		(*storage) << "}";
	}

public:
	TubeFrameFileDataSource(cv::FileNode node) : BaseFileDataSource(node)
	{
	}

	TubeFrameFileDataSource(cv::FileStorage *storage) : BaseFileDataSource(storage)
	{
	}
};

typedef cv::Ptr<TubeFrameFileDataSource> TubeFrameFileDataSourcePtr;

#ifndef QT
const char *TubeFrameFileDataSource::NODE_DATA = "data";
const char *TubeFrameFileDataSource::NODE_BOUNDING_BOX = "bbox";
const char *TubeFrameFileDataSource::NODE_FRAME_POSITION = "no";
#endif


}}}

#endif // FILEDATASOURCE_TUBEFRAMEFILEDATASOURCE_H
