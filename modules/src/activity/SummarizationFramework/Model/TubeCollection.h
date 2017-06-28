#pragma once
#ifndef TUBECOLLECTION_H
#define TUBECOLLECTION_H

#include "../CommonTypes.h"
#include "../Exceptions.h"
#include "../Persistence/IDataSource.h"
#include "../Persistence/IPersistable.h"
#include "Tube.h"
#include <algorithm>

namespace SummarizationFramework
{
namespace Model
{


class TubeCollection;
typedef cv::Ptr<TubeCollection> TubeCollectionPtr;

class TubeCollection : public Persistence::IPersistable<TubeCollection>
{
private:
	typedef std::vector<TubePtr> TubePtrVector;
	typedef cv::Ptr<TubePtrVector> TubePtrVectorPtr;

public:
	typedef TubePtrVector::iterator Iterator;

	class SortPredicate
	{
	public:
		virtual bool operator()(TubePtr & A, TubePtr & B) = 0;
	};

	typedef cv::Ptr<SortPredicate> SortPredicatePtr;

	class ByOriginalStartSortPredicate : public SortPredicate
	{
	public:
		virtual bool operator()(TubePtr & A, TubePtr & B)
		{
			return A->GetFrameSpan().From < B->GetFrameSpan().From;
		}
	};

private:
	class SortPredicateProxy
	{
	private:
		SortPredicatePtr predicate;

	public:
		SortPredicateProxy(SortPredicatePtr predicate)
		{
			CV_Assert(predicate != NULL);
			this->predicate = predicate;
		}

		bool operator()(TubePtr A, TubePtr B)
		{
			return predicate->operator()(A, B);
		}
	};

	TubePtrVectorPtr collection;

public:
	TubeCollection() : Persistence::IPersistable<TubeCollection>()
	{
		this->collection = new TubePtrVector();
		this->isLoaded = true;
	}

	TubeCollection(DataSourcePtr dataSource) : Persistence::IPersistable<TubeCollection>(dataSource)
	{
		this->collection = new TubePtrVector();
	}

	Iterator Begin()
	{
		lazyLoad();
		return collection->begin();
	}

	Iterator End()
	{
		lazyLoad();
		return collection->end();
	}

	void Insert(TubePtr tube)
	{
		lazyLoad();
		collection->insert(collection->end(), tube);
	}

	size_t Count()
	{
		return collection->size();
	}

	void Clear()
	{
		collection->clear();
	}

	void Sort(SortPredicatePtr predicate)
	{
		std::sort(collection->begin(), collection->end(), SortPredicateProxy(predicate));
	}

	TubeCollectionPtr Head(int count = -1)
	{
		TubeCollectionPtr newCollection = new TubeCollection();

		for(size_t i = 0; i < collection->size() && (i < (unsigned)count || count < 0); i++)
		{
			newCollection->Insert((*collection)[i]);
		}

		return newCollection;
	}

	virtual void Load()
	{
		if(dataSource != NULL && !isLoaded)
		{
			TubeCollectionPtr entity = dataSource->Load();
			
			collection = entity->collection;

			isLoaded = true;
		}
	}

	virtual void Save()
	{
		if(dataSource != NULL)
		{
			dataSource->Save(this);
		}
	}
};


}}

#endif // TUBECOLLECTION_H
