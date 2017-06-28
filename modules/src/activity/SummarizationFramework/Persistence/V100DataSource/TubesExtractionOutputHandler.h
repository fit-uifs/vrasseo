#pragma once
#ifndef V100DATASOURCE_TUBESEXTRACTOROUTPUTHANDLER_H
#define XXXXXXXXXXXXXX

#include <iostream>
#include "../../ImageProcessing/TubesExtraction/IOutputHandler.h"
#include "../../Model/TubeCollection.h"
#include "PathParams.h"
#include "TubeDataSource.h"
#include "TubeCollectionDataSource.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace V100DataSource
{


class TubesExtractionOutputHandler : public ImageProcessing::TubesExtraction::IOutputHandler
{
private:
	typedef std::vector<std::string> Filenames;
	typedef std::vector<int> Ids;

	PathParams params;
	Filenames files;
	Ids ids;

protected:
	virtual void takeTube(Model::TubePtr tube)
	{
		/*
		Model::TubeFrameCollectionPtr frfrfr = tube->GetFrames();
		if(tube->GetFrames()->size() < 10 || (*frfrfr)[0]->GetBoundingBox().y < 200 || (*frfrfr)[0]->GetBoundingBox().area() < 200) // TODO
		{
			return;
		}
		*/

		std::string filename = params.GetTubeDataXmlFilePath(tube->GetId());
		
		Model::Tube::DataSourcePtr dataSource = new TubeDataSource(filename, params);		
		tube->SetDataSource(dataSource);
		
		tube->Save();
		files.push_back(filename);
		ids.push_back(tube->GetId());
	}

public:
	TubesExtractionOutputHandler(PathParams params)
	{
		this->params = params;
	}

	virtual void Finish()
	{
		Model::TubeCollectionPtr tubes = new Model::TubeCollection();

		for(size_t i = 0; i < files.size(); i++)
		{
			Model::TubePtr tube = new Model::Tube(NULL, FrameSpan(), files[i], ids[i]);
			tubes->Insert(tube);
		}

		Model::TubeCollection::DataSourcePtr dataSource = new TubeCollectionDataSource(params.GetTubesXmlFilePath(), params);
		tubes->SetDataSource(dataSource);
		tubes->Save();
	}
};

typedef cv::Ptr<TubesExtractionOutputHandler> TubesExtractionOutputHandlerPtr;


}}}

#endif // V100DATASOURCE_TUBESEXTRACTOROUTPUTHANDLER_H
