#pragma once
#ifndef V100DATASOURCE_TUBEFRAMEDATASOURCE_H
#define V100DATASOURCE_TUBEFRAMEDATASOURCE_H

#include "../../Model/TubeFrame.h"
#include "XmlBasedDataSource.h"
#include "TubeFrameDataDataSource.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace V100DataSource
{


class TubeFrameDataSource : public XmlBasedDataSource<Model::TubeFrame>
{
protected:
	static const char *NODE_ROOT;
	static const char *NODE_BOUNDING_BOX;
	static const char *NODE_FRAME_POSITION;
	
	// for loading
	Model::VideoSequencePtr maskSequence;
	int maskFrameNumber;

	// for saving
	cv::VideoWriter *maskWriter;

	virtual Model::TubeFramePtr internalLoad(cv::FileNode node)
	{
		cv::FileNode rootNode = CheckNode(node[NODE_ROOT]);
		cv::FileNode boundingBoxNode = CheckNode(rootNode[NODE_BOUNDING_BOX]);
		cv::FileNode framePositionNode = CheckNode(rootNode[NODE_FRAME_POSITION]);

		std::vector<int> boundingBoxVector;
		boundingBoxNode >> boundingBoxVector;
		cv::Rect boundingBox(boundingBoxVector[0], boundingBoxVector[1], boundingBoxVector[2], boundingBoxVector[3]);
		
		FramePosition framePosition;
		framePositionNode >> framePosition;

		Model::TubeFrameDataPtr data = new Model::TubeFrameData(new TubeFrameDataDataSource(maskSequence, maskFrameNumber));
		maskSequence = NULL;

		return new Model::TubeFrame(data, boundingBox, framePosition);
	}

	virtual void internalSave(cv::FileStorage *storage, Model::TubeFrame *entity)
	{
		(*storage) << NODE_ROOT << "{";
		
		(*storage) << NODE_FRAME_POSITION << entity->GetFramePosition();
		(*storage) << NODE_BOUNDING_BOX << entity->GetBoundingBox();

		Model::TubeFrameDataPtr data = entity->GetData();
		
		Model::TubeFrameData::DataSourcePtr oldDataSource = data->GetDataSource();
		data->SetDataSource(new TubeFrameDataDataSource(maskWriter));
		data->Save();
		data->SetDataSource(oldDataSource);

		(*storage) << "}"; // NODE_ROOT
	}

public:
	TubeFrameDataSource(PathParams params, cv::FileNode node, Model::VideoSequencePtr maskSequence, int maskFrameNumber) : XmlBasedDataSource(params, node)
	{
		this->maskSequence = maskSequence;
		this->maskFrameNumber = maskFrameNumber;
	}

	TubeFrameDataSource(PathParams params, cv::FileStorage *storage, cv::VideoWriter *maskWriter = NULL) : XmlBasedDataSource(params, storage)
	{
		this->maskWriter = maskWriter;
	}
};

typedef cv::Ptr<TubeFrameDataSource> TubeFrameDataSourcePtr;

#ifndef QT
const char *TubeFrameDataSource::NODE_ROOT = "frame";
const char *TubeFrameDataSource::NODE_BOUNDING_BOX = "bb";
const char *TubeFrameDataSource::NODE_FRAME_POSITION = "no";
#endif


}}}

#endif // V100DATASOURCE_TUBEFRAMEDATASOURCE_H
