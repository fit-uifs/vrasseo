#pragma once
#ifndef COLLECTIONOUTPUTHANDLER_H
#define COLLECTIONOUTPUTHANDLER_H

#include "../../Model/TubeCollection.h"
#include "IOutputHandler.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace TubesExtraction
{


class CollectionOutputHandler : public IOutputHandler
{
private:
	Model::TubeCollectionPtr collection;

protected:
	virtual void takeTube(Model::TubePtr tube)
	{
		collection->Insert(tube);
	}

public:
	CollectionOutputHandler() : IOutputHandler()
	{
		collection = new Model::TubeCollection();
	}

	virtual ~CollectionOutputHandler()
	{
	}

	Model::TubeCollectionPtr GetTubes()
	{
		return collection;
	}
};

typedef cv::Ptr<CollectionOutputHandler> CollectionOutputHandlerPtr;


}}}

#endif // COLLECTIONOUTPUTHANDLER_H
