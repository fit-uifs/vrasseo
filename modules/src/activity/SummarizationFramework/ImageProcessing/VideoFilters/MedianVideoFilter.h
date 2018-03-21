#pragma once
#ifndef MEDIANVIDEOFILTER_H
#define MEDIANVIDEOFILTER_H

#include <algorithm>
#include <sstream>
#include "../../CommonTypes.h"
#include "../../Model/VideoSequence.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace VideoFilters
{


class MedianVideoFilter : public Model::IVideoSource
{
private:
	template<typename T>
	class SparseHistogram
	{
	private:
		typedef std::map<T, int> Bins;
		
		Bins bins;
		int totalCount;

	public:
		SparseHistogram()
		{
			totalCount = 0;
			bins = Bins();
		}

		void AddCount(T bin, int count)
		{
			if(bins.find(bin) != bins.end())
			{
				bins[bin] += count;
			}
			else
			{
				bins[bin] = count;
			}

			totalCount += count;

			if(bins[bin] == 0)
			{
				RemoveBin(bin);
			}
		}

		void RemoveBin(T bin)
		{
			if(bins.find(bin) != bins.end())
			{
				totalCount -= bins[bin];
				bins.erase(bin);
			}
		}

		void Clear()
		{
			bins.clear();
			totalCount = 0;
		}

		uchar FindMedian()
		{
			int currentCount = 0;
			int countToReach = totalCount/2;
			
			// assumes std::map is sorted
			for(Bins::iterator it = bins.begin(); it != bins.end(); it++)
			{
				currentCount += it->second;
			
				if(currentCount > countToReach)
				{
					return it->first;
				}
			}

			return 0;
		}

		SparseHistogram<T> & operator+(SparseHistogram<T> & second)
		{
			for(Bins::iterator it = second.bins.begin(); it != second.bins.end(); it++)
			{
				AddCount(it->first, it->second);
			}

			return *this;
		}

		SparseHistogram<T> & operator+=(SparseHistogram<T> & second)
		{
			return *this + second;
		}

		SparseHistogram<T> & operator-(SparseHistogram<T> & second)
		{
			for(Bins::iterator it = second.bins.begin(); it != second.bins.end(); it++)
			{
				AddCount(it->first, -it->second); // minus!
			}

			return *this;
		}

		SparseHistogram<T> & operator-=(SparseHistogram<T> & second)
		{
			return *this - second;
		}

		std::string ToString()
		{
			std::stringstream str;

			for(Bins::iterator it = bins.begin(); it != bins.end(); it++)
			{
				str << (int)it->first << "(" << (int)it->second << ") ";
			}

			str << "\n";

			return str.str();
		}
	};

	typedef cv::Ptr<SparseHistogram<uchar> > SparseHistogramPtr;
	typedef std::vector<Frame> Frames;
	typedef cv::Ptr<Frames> FramesPtr;

	Model::IVideoSourcePtr source;
	
	FramesPtr frames;

	SparseHistogramPtr *histograms;
	size_t histogramsSize;

	size_t medianSize;	
	int medianIndex;
	
	FramePtr currentFrame;
	int currentFrameNumber;
	bool currentFrameReady;

	void allocCurrentFrame(const Frame & frame)
	{
		CV_Assert(!frame.empty());

		currentFrame = new Frame(frame.rows, frame.cols, frame.type());
	}

	void allocHistograms(const Frame & frame)
	{
		CV_Assert(!frame.empty());

		histogramsSize = frame.rows*frame.cols*frame.channels();
		histograms = new SparseHistogramPtr [histogramsSize];
		
		for(size_t i = 0; i < histogramsSize; i++)
		{
			histograms[i] = new SparseHistogram<uchar>();
		}
	}

	void updateHistograms(const Frame & frame, int cnt)
	{
		for(unsigned int i = 0; i < histogramsSize; i++) {
			histograms[i]->AddCount(frame.data[i], cnt);
		}		
	}

	void insertFrame(const Frame & frame)
	{
		if(currentFrame == NULL)
		{
			allocCurrentFrame(frame);
		}

		if(histograms == NULL)
		{
			allocHistograms(frame);
		}

		if(frames->size() >= medianSize)
		{
			Frame frameToOverwrite = *(frames->begin());
			
			updateHistograms(frameToOverwrite, -1);
			updateHistograms(frame, 1);

			frame.copyTo(frameToOverwrite);

			frames->erase(frames->begin());
			frames->push_back(frameToOverwrite);			
		}
		else
		{		
			frames->push_back(frame.clone());
			updateHistograms(frame, 1);
			std::cout << "init->"<<frames->size() << "\n";
		}

		currentFrameReady = false;
	}

	void initializeState()
	{
		while(frames->size() < medianSize && source->Next())
		{
			insertFrame(source->GetFrame());
		}

		if(frames->size() < medianSize)
		{
			medianSize = frames->size();
			medianIndex = medianSize/2;
		}
	}

	void prepareCurrentFrame()
	{
		if(frames->size() == 0)
		{
			return;
		}
		
		for(unsigned int i = 0; i < histogramsSize; i++) {
			currentFrame->data[i] = histograms[i]->FindMedian();
		}

		currentFrameReady = true;
	}

protected:
	virtual bool seek(int frame)
	{
		throw NotSupportedException("MedianVideoFilter does not support seek.");
	}

public:
	MedianVideoFilter(Model::IVideoSourcePtr source, size_t medianSize)
	{
		CV_Assert((medianSize % 2) != 0);

		this->source = source;
		this->frames = new Frames();
		this->histograms = NULL;
		this->medianSize = medianSize;
		this->medianIndex = medianSize/2;
		this->currentFrame = NULL;
		this->currentFrameNumber = 0;
		this->currentFrameReady = false;

		initializeState();
	}

	virtual ~MedianVideoFilter()
	{
	}

	virtual void Reset()
	{
		throw NotSupportedException("MedianVideoFilter does not support reset.");
	}

	virtual bool Next()
	{
		currentFrameNumber++;

		if(currentFrameNumber > medianIndex)
		{
			if(!source->Next())
			{
				return currentFrameNumber <= GetLength(); // TODO predelat, aby nebyla potreba znat delka sekvence???
			}
			else
			{
				insertFrame(source->GetFrame());
			}
		}

		return true;
	}

	virtual Frame GetFrame()
	{
		if(!currentFrameReady)
		{
			prepareCurrentFrame();
		}

		return currentFrame.obj->clone();
	}

	virtual int GetFrameNumber()
	{
		return currentFrameNumber;
	}

	virtual int GetLength()
	{
		return source->GetLength();
	}

	int GetMedianSize()
	{
		return medianSize;
	}
};

typedef cv::Ptr<MedianVideoFilter> MedianVideoFilterPtr;


}}}

#endif // MEDIANVIDEOFILTER_H
