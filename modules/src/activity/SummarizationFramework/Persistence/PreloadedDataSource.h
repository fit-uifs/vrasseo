#pragma once
#ifndef PRELOADEDDATASOURCE_H
#define PRELOADEDDATASOURCE_H

#include "../CommonTypes.h"
#include "IDataSource.h"

namespace SummarizationFramework
{
namespace Persistence
{


template<typename T>
class PreloadedDataSource : public IDataSource<T>
{
private:
	TPtr entity;

public:
	PreloadedDataSource(TPtr entity)
	{
		this->entity = entity;
	}
	
	virtual TPtr Load()
	{
		return entity;
	}

	virtual void Save(T *entity)
	{
		throw NotSupportedException("PreloadedDataSource does not support save.");
	}
};


}}

#endif // PRELOADEDDATASOURCE_H
