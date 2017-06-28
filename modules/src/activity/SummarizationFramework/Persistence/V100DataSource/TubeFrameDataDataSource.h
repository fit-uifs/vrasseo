#pragma once
#ifndef V100DATASOURCE_TUBEFRAMEDATADATASOURCE_H
#define V100DATASOURCE_TUBEFRAMEDATADATASOURCE_H

#include <opencv2/highgui/highgui.hpp>
#include "../../Model/TubeFrameData.h"
#include "../../Model/VideoSequence.h"
#include "../IDataSource.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace V100DataSource
{


class TubeFrameDataDataSource : public IDataSource<Model::TubeFrameData>
{
protected:
	// for loading
	Model::VideoSequencePtr maskSequence;
	int maskFrameNumber;

	// for saving
	cv::VideoWriter *maskWriter;

public:
	TubeFrameDataDataSource(Model::VideoSequencePtr maskSequence, int maskFrameNumber)
	{
		this->maskSequence = maskSequence;
		this->maskFrameNumber = maskFrameNumber;
	}

	TubeFrameDataDataSource(cv::VideoWriter *maskWriter = NULL)
	{
		this->maskWriter = maskWriter;
	}

	virtual Model::TubeFrameDataPtr Load()
	{
		maskSequence->Seek(maskFrameNumber);
		FrameMask mask = maskSequence->GetFrame();
		
		maskSequence = NULL;
		return new Model::TubeFrameData(mask);
	}

	virtual void Save(Model::TubeFrameData *entity)
	{
		if(maskWriter == NULL)
		{
			return;
		}

		maskWriter->write(entity->GetMask());
	}
};

typedef cv::Ptr<TubeFrameDataDataSource> TubeFrameDataDataSourcePtr;


}}}

#endif // V100DATASOURCE_TUBEFRAMEDATADATASOURCE_H
