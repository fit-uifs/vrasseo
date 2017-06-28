#pragma once
#ifndef FILEDATASOURCE_TUBEFILEDATASOURCE_H
#define FILEDATASOURCE_TUBEFILEDATASOURCE_H

#include "TubeFrameFileDataSource.h"
#include "../../Model/Tube.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace FileDataSource
{


class TubeFileDataSource : public BaseFileDataSource<Model::Tube>
{
protected:
	static const char *NODE_FRAMES;
	static const char *NODE_FRAMESPAN_FROM;
	static const char *NODE_FRAMESPAN_TO;
	static const char *NODE_FILENAME;
	static const char *NODE_ID;

	virtual Model::TubePtr internalLoad(cv::FileNode node)
	{
		cv::FileNode framesNode = checkedNode(node[NODE_FRAMES]);
		cv::FileNode fromFrameNode = checkedNode(node[NODE_FRAMESPAN_FROM]);
		cv::FileNode toFrameNode = checkedNode(node[NODE_FRAMESPAN_TO]);
		cv::FileNode fileNameNode = checkedNode(node[NODE_FILENAME]);
		cv::FileNode idNode = checkedNode(node[NODE_ID]);

		int id;
		idNode >> id;

		int fromFrame, toFrame;
		fromFrameNode >> fromFrame;
		toFrameNode >> toFrame;

		FrameSpan frameSpan(fromFrame, toFrame);

		std::string fileName;
		fileNameNode >> fileName;

		Model::TubeFrameCollectionPtr frames = new Model::TubeFrameCollection();
		for(cv::FileNodeIterator it = framesNode.begin(); it != framesNode.end(); it++)
		{
			Model::TubeFramePtr frame = new Model::TubeFrame(new TubeFrameFileDataSource(*it));
			frame->Load();
			frame->SetDataSource(NULL);

			frames->push_back(frame);
		}

		return new Model::Tube(frames, frameSpan, fileName, id);
	}

	virtual void internalSave(cv::FileStorage *storage, Model::Tube *entity)
	{
		entity->Load();

		(*storage) << NODE_ID << entity->GetId();
		
		(*storage) << NODE_FILENAME << entity->GetFileName();

		(*storage) << NODE_FRAMESPAN_FROM << entity->GetFrameSpan().From;
		(*storage) << NODE_FRAMESPAN_TO << entity->GetFrameSpan().To;

		(*storage) << NODE_FRAMES << "[";
			Model::TubeFrameCollectionPtr frames = entity->GetFrames();
			for(Model::TubeFrameCollection::iterator it = frames->begin(); it != frames->end(); it++)
			{
				(*storage) << "{";

				Model::TubeFramePtr frame = *it;
				frame->Load();

				Model::TubeFrame::DataSourcePtr oldDataSource = frame->GetDataSource();
				frame->SetDataSource(new TubeFrameFileDataSource(storage));
				frame->Save();
				frame->SetDataSource(oldDataSource);

				(*storage) << "}";
			}
		(*storage) << "]";
	}

public:
	TubeFileDataSource(cv::FileNode node) : BaseFileDataSource(node)
	{
	}

	TubeFileDataSource(cv::FileStorage *storage) : BaseFileDataSource(storage)
	{
	}
};

typedef cv::Ptr<TubeFileDataSource> TubeFileDataSourcePtr;

#ifndef QT
const char *TubeFileDataSource::NODE_FRAMES = "frames";
const char *TubeFileDataSource::NODE_FRAMESPAN_FROM = "from";
const char *TubeFileDataSource::NODE_FRAMESPAN_TO = "to";
const char *TubeFileDataSource::NODE_FILENAME = "file";
const char *TubeFileDataSource::NODE_ID = "id";
#endif

}}}

#endif // FILEDATASOURCE_TUBEFILEDATASOURCE_H
