#pragma once
#ifndef IDATASOURCE_H
#define IDATASOURCE_H


#include "../CommonTypes.h"

namespace SummarizationFramework
{
namespace Persistence
{


template<typename T>
class IDataSource
{
public:
	typedef cv::Ptr<T> TPtr;

	virtual TPtr Load() = 0;
	virtual void Save(T *entity) = 0;
};


}}

#endif // IDATASOURCE_H
