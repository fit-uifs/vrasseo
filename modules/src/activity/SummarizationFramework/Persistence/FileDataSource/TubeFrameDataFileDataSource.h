#pragma once
#ifndef FILEDATASOURCE_TUBEFRAMEDATAFILEDATASOURCE_H
#define FILEDATASOURCE_TUBEFRAMEDATAFILEDATASOURCE_H

#include "BaseFileDataSource.h"
#include "../../Model/TubeFrameData.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace FileDataSource
{


class TubeFrameDataFileDataSource : public BaseFileDataSource<Model::TubeFrameData>
{
protected:
	static const char *NODE_MASK;

	virtual Model::TubeFrameDataPtr internalLoad(cv::FileNode node)
	{
		cv::FileNode maskNode = checkedNode(node[NODE_MASK]);

		cv::Mat mask;
		maskNode >> mask;

		mask *= 255;

		return new Model::TubeFrameData(mask);
	}

	virtual void internalSave(cv::FileStorage *storage, Model::TubeFrameData *entity)
	{
		entity->Load();

		cv::Mat mask = entity->GetMask();
		mask /= 255;

		(*storage) << NODE_MASK << entity->GetMask();
	}

public:
	TubeFrameDataFileDataSource(cv::FileNode node) : BaseFileDataSource(node)
	{
	}

	TubeFrameDataFileDataSource(cv::FileStorage *storage) : BaseFileDataSource(storage)
	{
	}
};

typedef cv::Ptr<TubeFrameDataFileDataSource> TubeFrameDataFileDataSourcePtr;

#ifndef QT
const char *TubeFrameDataFileDataSource::NODE_MASK = "mask";
#endif


}}}

#endif // FILEDATASOURCE_TUBEFRAMEDATAFILEDATASOURCE_H
