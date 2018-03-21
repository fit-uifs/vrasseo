#pragma once
#ifndef MOGTUBESEXTRACTOR_H
#define MOGTUBESEXTRACTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../../Helpers/Geometry.h"
#include "ITubesExtractor.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace TubesExtraction
{


class MogTubesExtractor : public ITubesExtractor
{
private:
	double minFmeasure;
	
	typedef std::vector<cv::Point> PointVector;

	class Contour : private std::vector<cv::Point> {
	private:
		FrameMask mask;
		FramePosition number;

	public:
		Contour(PointVector contour, Frame frame, FramePosition number) : PointVector(contour)
		{
			mask = cv::Mat::zeros(frame.rows, frame.cols, CV_8U);

			std::vector<PointVector> tmp;
			tmp.push_back(*this);
			cv::drawContours(mask, tmp, 0, cv::Scalar(255,255,255), CV_FILLED, 8);

			dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			//dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			//dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);

			this->number = number;
		}

		double GetArea() const
		{
			return cv::contourArea(*this, false);
		}

		cv::Rect GetBoundingBox() const
		{
			return cv::boundingRect(*this);
		}

		FrameMask GetMask() const
		{
			return mask;
		}

		FramePosition GetFrameNumber() const
		{
			return number;
		}

		double GetOverlapFMeasure(Contour & second) const
		{
			cv::Rect bb1 = GetBoundingBox();
			cv::Rect bb2 = second.GetBoundingBox();
			
			return Helpers::Geometry::Fmeasure(bb1, bb2);
		
			return 0;
		}
	};

	typedef std::vector<Contour> ContourVector;

	class ContourFactory {
	public:
		static ContourVector Create(std::vector<PointVector> contours, Frame frame, FramePosition number)
		{
			ContourVector result;

			for(size_t i = 0; i< contours.size(); i++ ) {
				Contour c(contours[i], frame, number);
				result.push_back(c);
			}

			return result;
		}
	};

	class ContourTrack : public ContourVector {
	public:
		bool alive;

		ContourTrack() : ContourVector()
		{
		}

		ContourTrack(Contour contour) : ContourVector()
		{
			push_back(contour);
		}
	};

	typedef cv::Ptr<ContourTrack> ContourTrackPtr;
	typedef std::vector<ContourTrackPtr> ContourTrackPtrVector;

	class ContourTracker {
	public:
		class EventsHandler
		{
		public:
			virtual void TubeReady(ContourTrackPtr contour) = 0;
		};

		typedef cv::Ptr<EventsHandler> EventsHandlerPtr;
	
	private:
		EventsHandlerPtr eventsHandler;
		ContourTrackPtrVector currentTracks;
		double minFmeasure;

		void setCurrentTracksNotAlive()
		{
			for(ContourTrackPtrVector::iterator it = currentTracks.begin(); it != currentTracks.end(); it++)
			{
				(*it)->alive = false;
			}
		}

		void removeFinishedTracks()
		{
			ContourTrackPtrVector tracksToKeep;

			for(size_t i = 0; i < currentTracks.size(); i++)
			{
				if(currentTracks[i]->alive)
				{
					tracksToKeep.push_back(currentTracks[i]);
				}
				else if(eventsHandler != NULL)
				{
					eventsHandler->TubeReady(currentTracks[i]);
				}
			}

			currentTracks = tracksToKeep;
		}

	public:
		ContourTracker(EventsHandlerPtr eventsHandler, double minFmeasure)
		{
			this->eventsHandler = eventsHandler;
			this->minFmeasure = minFmeasure;
		}

		void Process(ContourVector contours)
		{
			setCurrentTracksNotAlive();
		
			for(ContourVector::iterator itContour = contours.begin(); itContour != contours.end(); itContour++)
			{
				double bestFitOverlap = 0;
				ContourTrackPtr bestFitTrack;

				// find best matching contour from previous frame
				for(ContourTrackPtrVector::iterator itTrack = currentTracks.begin(); itTrack != currentTracks.end(); itTrack++)
				{
					if(!(*itTrack)->alive)
					{
						ContourTrackPtr currentTrack = *itTrack;
						double overlap = itContour->GetOverlapFMeasure((*currentTrack)[currentTrack->size()-1]);

						if(overlap > bestFitOverlap)
						{
							bestFitOverlap = overlap;
							bestFitTrack = *itTrack;
						}
					}
				}

				// some match found -> extend tube
				if(bestFitOverlap > minFmeasure)
				{
					bestFitTrack->push_back(*itContour);
					bestFitTrack->alive = true;
				}
				// no match -> start a new tube
				else
				{
					ContourTrackPtr newTrack = new ContourTrack(*itContour);
					newTrack->alive = true;
					currentTracks.push_back(newTrack);
				}
			}

			removeFinishedTracks();
		}

		void Finish()
		{
			setCurrentTracksNotAlive();
			removeFinishedTracks();
		}

		ContourTrackPtrVector GetCurrentTracks() const
		{
			return currentTracks;
		}
	};

	class TrackerEventsHandler : public ContourTracker::EventsHandler
	{
	private:
		IOutputHandlerPtr outputHandler;
		std::string filename;

	public:
		TrackerEventsHandler(IOutputHandlerPtr outputHandler, std::string filename)
		{
			this->outputHandler = outputHandler;
			this->filename = filename;
		}

		virtual void TubeReady(ContourTrackPtr contour)
		{
			ContourVector contours = *contour;

			if(contours.size() == 0)
			{
				return;
			}
			
			FrameSpan frameSpan(contours[0].GetFrameNumber(), contours[0].GetFrameNumber());
			Model::TubeFrameCollectionPtr frames = new Model::TubeFrameCollection();

			// render mask for all frames
			for(ContourVector::iterator co = contours.begin(); co != contours.end(); co++)
			{			
				FrameMask mask = co->GetMask();
				Model::TubeFrameDataPtr data = new Model::TubeFrameData(mask);
				Model::TubeFramePtr frame = new Model::TubeFrame(data, co->GetBoundingBox(), co->GetFrameNumber());
				
				frames->push_back(frame);
				frameSpan.To = co->GetFrameNumber();
			}
			
			// create tube and send it to output
			Model::TubePtr tube = new Model::Tube(frames, frameSpan, filename);
			outputHandler->Take(tube);
		}
	};

	typedef cv::Ptr<TrackerEventsHandler> TrackerEventsHandlerPtr;

public:
	MogTubesExtractor(IOutputHandlerPtr outputHandler) : ITubesExtractor(outputHandler)
	{
		minFmeasure = 0.33; // TODO udelat nastavitelne
	}

	virtual ~MogTubesExtractor()
	{
	}

	virtual void ProcessVideoSource(Model::IVideoSourcePtr video, std::string filename)
	{
		CV_Assert(video != NULL);
		CV_Assert(GetOutputHandler() != NULL);
		CV_Assert(!filename.empty());
		
		TrackerEventsHandlerPtr eventsHandler = new TrackerEventsHandler(GetOutputHandler(), filename);

		ContourTracker tracker(eventsHandler, minFmeasure);
		
        cv::Ptr<cv::BackgroundSubtractorMOG2> backgroundSubtractor =
                cv::createBackgroundSubtractorMOG2(500, 16, false);

		Frame frame;
		FramePosition frameNumber = 0;

		while(video->Next() && !isStopRequested())
		{
			frameNumber++;
			frame = video->GetFrame();
			
			Frame frameResized;
			frame.copyTo(frameResized);
			cv::resize(frame, frameResized, cv::Size(), 0.5, 0.5); // TODO meritko ne natvrdo

			FrameMask mask;
            backgroundSubtractor->apply(frameResized, mask);

			erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);
			dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2, 1, 1);

			cv::resize(mask, mask, cv::Size(frame.cols, frame.rows));

			std::vector<PointVector> contours;
			std::vector<cv::Vec4i> hierarchy;

			cv::findContours(mask, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

			ContourVector ccc = ContourFactory::Create(contours, frame, frameNumber);
			tracker.Process(ccc);
		}

		tracker.Finish();
		GetOutputHandler()->Finish();
	}
};

typedef cv::Ptr<MogTubesExtractor> MogTubesExtractorPtr;


}}}

#endif // MOGTUBESEXTRACTOR_H
