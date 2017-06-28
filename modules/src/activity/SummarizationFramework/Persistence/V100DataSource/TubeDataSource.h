#pragma once
#ifndef V100DATASOURCE_TUBEDATASOURCE_H
#define V100DATASOURCE_TUBEDATASOURCE_H

#include "../../Model/Tube.h"
#include "FileBasedDataSource.h"
#include "TubeFrameDataSource.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace V100DataSource
{


class TubeDataSource : public FileBasedDataSource<Model::Tube>
{
protected:
	static const char *NODE_ROOT;
	static const char *NODE_HEADER;
	static const char *NODE_HEADER_ID;
	static const char *NODE_HEADER_START;
	static const char *NODE_HEADER_END;
	static const char *NODE_HEADER_FILE_VIDEO;
	static const char *NODE_HEADER_FILE_MASK;
	static const char *NODE_DATA;

	virtual Model::TubePtr internalLoad(cv::FileNode node)
	{
		cv::FileNode rootNode = CheckNode(node[NODE_ROOT]);
		cv::FileNode headerNode = CheckNode(rootNode[NODE_HEADER]);
		cv::FileNode dataNode = CheckNode(rootNode[NODE_DATA]);

		// process header
		cv::FileNode idNode = CheckNode(headerNode[NODE_HEADER_ID]);
		cv::FileNode fromFrameNode = CheckNode(headerNode[NODE_HEADER_START]);
		cv::FileNode toFrameNode = CheckNode(headerNode[NODE_HEADER_END]);
		cv::FileNode videoFileNode = CheckNode(headerNode[NODE_HEADER_FILE_VIDEO]);
		cv::FileNode maskFileNode = CheckNode(headerNode[NODE_HEADER_FILE_MASK]);		

		int fromFrame;
		int toFrame;
		int id;

		fromFrameNode >> fromFrame;
		toFrameNode >> toFrame;
		idNode >> id;
		
		std::string videoFilename;
		std::string maskFilename;

		videoFileNode >> videoFilename;
		maskFileNode >> maskFilename;

		FrameSpan frameSpan(fromFrame, toFrame);
		
		// process data
		Model::TubeFrameCollectionPtr frames = new Model::TubeFrameCollection();				
		
		int maskFrameNumber = 1;
		Model::VideoSequencePtr maskSequence = new Model::VideoSequence(maskFilename);

		for(cv::FileNodeIterator it = dataNode.begin(); it != dataNode.end(); it++)
		{
			Model::TubeFramePtr frame = new Model::TubeFrame(new TubeFrameDataSource(params, *it, maskSequence, maskFrameNumber++));
			frame->Load();
			frame->SetDataSource(NULL);

			frames->push_back(frame);
		}

		maskSequence = NULL;

		return new Model::Tube(frames, frameSpan, videoFilename, id);
	}

	virtual void internalSave(cv::FileStorage *storage, Model::Tube *entity)
	{
		(*storage) << NODE_ROOT << "{";

		(*storage) << NODE_HEADER << "{";
			(*storage) << NODE_HEADER_ID << entity->GetId();
			(*storage) << NODE_HEADER_START << entity->GetFrameSpan().From;
			(*storage) << NODE_HEADER_END << entity->GetFrameSpan().To;
			(*storage) << NODE_HEADER_FILE_VIDEO << entity->GetFileName();
			(*storage) << NODE_HEADER_FILE_MASK << params.GetTubeMaskFilePath(entity->GetId());
		(*storage) << "}"; // NODE_HEADER

		Model::TubeFrameCollectionPtr frames = entity->GetFrames();

		cv::Ptr<cv::VideoWriter> maskWriter = NULL;
		
		if(frames->size() > 0 && frames[0].size() > 0 && !frames[0][0]->GetData()->GetMask().empty())
		{
			FrameMask mask = frames[0][0]->GetData()->GetMask();
			cv::Size size(mask.cols, mask.rows);
			maskWriter = new cv::VideoWriter(params.GetTubeMaskFilePath(entity->GetId()), params.GetMaskFourCc(), params.GetMaskFps(), size);
		}

		(*storage) << NODE_DATA << "[";	
			for(Model::TubeFrameCollection::iterator it = frames->begin(); it != frames->end(); it++)
			{
				(*storage) << "{";

				Model::TubeFramePtr frame = *it;
				frame->Load();

				Model::TubeFrame::DataSourcePtr oldDataSource = frame->GetDataSource();
				frame->SetDataSource(new TubeFrameDataSource(params, storage, maskWriter));
				frame->Save();
				frame->SetDataSource(oldDataSource);

				(*storage) << "}";
			}
		(*storage) << "]"; // NODE_DATA

		maskWriter->release();

		(*storage) << "}"; // NODE_ROOT
	}

public:
	TubeDataSource(std::string filename, PathParams params) : FileBasedDataSource(filename, params)
	{
	}
};

typedef cv::Ptr<TubeDataSource> TubeDataSourcePtr;

#ifndef QT
const char *TubeDataSource::NODE_ROOT = "tube";
const char *TubeDataSource::NODE_HEADER = "header";
const char *TubeDataSource::NODE_HEADER_ID = "id";
const char *TubeDataSource::NODE_HEADER_START = "start";
const char *TubeDataSource::NODE_HEADER_END = "end";
const char *TubeDataSource::NODE_HEADER_FILE_VIDEO = "file_video";
const char *TubeDataSource::NODE_HEADER_FILE_MASK = "file_mask";
const char *TubeDataSource::NODE_DATA = "data";
#endif

}}}

#endif // V100DATASOURCE_TUBEDATASOURCE_H
