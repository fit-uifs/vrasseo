#pragma once
#ifndef SYNOPSISRENDERER_H
#define SYNOPSISRENDERER_H

#include <map>
#include <opencv2/highgui/highgui.hpp>
#include "../ImageProcessing/FrameFilters/SimpleFrameBlender.h"
#include "../ImageProcessing/VideoFilters/IntervalVideoFilter.h"
#include "../Model/VideoSequence.h"
#include "../Helpers/ScenarioIndexing/SimpleScenarioIndexer.h"
#include "IRenderer.h"

namespace SummarizationFramework
{
namespace Presentation
{


class SynopsisRenderer : public IRenderer
{
private:
	typedef std::map<Model::Scenario::TubeMappingPtr, ImageProcessing::VideoFilters::IntervalVideoFilterPtr> ActiveMappingsMap;

	class ActiveMappings : public ActiveMappingsMap
	{
	public:
		ActiveMappings() : ActiveMappingsMap()
		{
		}

		ImageProcessing::VideoFilters::IntervalVideoFilterPtr lookAhead(Model::Scenario::TubeMappingPtr mapping)
		{
			// if item sequence exists, just return it
			iterator it = find(mapping);
			if(it != end())
			{
				return it->second;
			}
			
			Model::VideoSequencePtr videoSequence = new Model::VideoSequence(mapping->GetTube()->GetFileName());
			ImageProcessing::VideoFilters::IntervalVideoFilterPtr interval = new ImageProcessing::VideoFilters::IntervalVideoFilter(videoSequence, mapping->GetTube()->GetFrameSpan());

			(*this)[mapping] = interval;
			return interval;
		}

		void removeInactive(FramePosition position)
		{
			ActiveMappingsMap newMappings;

			for(iterator it = begin(); it != end(); it++)
			{
				if(it->first->GetLastFramePosition() > position)
				{
					newMappings[it->first] = it->second;
				}
				else
				{					
					// MML
					Model::Scenario::TubeMappingPtr mapping = it->second;
					if(mapping != NULL)
					{					
						/*
						Model::TubeFrameCollectionPtr tubeFrames = mapping->GetTube()->GetFrames();
					
						for(Model::TubeFrameCollection::iterator itFrames = tubeFrames->begin(); itFrames != tubeFrames->end(); itFrames++)
						{
							(*itFrames)->GetData()->GetMask();
						}
						*/
						ImageProcessing::VideoFilters::IntervalVideoFilterPtr tubeSequence = it->second;
						Model::TubeFrameCollectionPtr tubeFrames = mapping->GetTube()->GetFrames();

						if(tubeFrames != NULL)
						{
							while(tubeSequence->Next())
							{
								Frame tubeFrame = tubeSequence->GetFrame();
								FrameMask tubeMask = (*tubeFrames)[tubeSequence->GetFrameNumber()-1]->GetData()->GetMask();
							}
						}

						//it->second.delete_obj();
						//it->second = NULL;
						//delete it->second;
						//it->second.release();
					}
				}
			}

			clear();
			insert(newMappings.begin(), newMappings.end());
		}
	};

	typedef cv::Ptr<ActiveMappings> ActiveMappingsPtr;

	ImageProcessing::FrameFilters::FrameBlenderPtr blender;
	Helpers::ScenarioIndexing::IScenarioIndexerPtr indexer;

	void init(Model::ScenarioPtr scenario, Model::IVideoSourcePtr background)
	{
		if(blender == NULL)
		{
			SetFrameBlender(new ImageProcessing::FrameFilters::SimpleFrameBlender());
		}

		if(indexer == NULL)
		{
			SetScenarioIndexer(new Helpers::ScenarioIndexing::SimpleScenarioIndexer());
		}

		indexer->SetScenario(scenario);
	}

	virtual void internalRender(Model::ScenarioPtr scenario, Model::IVideoSourcePtr background, std::string filename)
	{
		init(scenario, background);

		FramePosition n = 0;
		ActiveMappingsPtr activeMappings = new ActiveMappings();
		cv::VideoWriter *writer = NULL;

		try
		{
			while(background->Next() && n <= scenario->GetLength())
			{
				Frame frame = background->GetFrame();

				if(writer == NULL) // initialization
				{
					writer = new cv::VideoWriter(filename, params.fourcc, params.fps, cv::Size(frame.cols, frame.rows));
				}
								
				Model::Scenario::TubeMappingCollection mappedTubes = indexer->GetTubes(++n);
				
				/*
				std::cout << "FRAME #" << n << "\n";
				std::cout << "--mapped " << mappedTubes.size() << " tubes (loaded " << activeMappings->size() << ")\n";
				for(Model::Scenario::TubeMappingCollection::iterator it = mappedTubes.begin(); it != mappedTubes.end(); it++)
				{
					std::cout << "-----id #" << (*it)->GetTube()->GetId() << ": " << (*it)->GetTube()->GetFrameSpan().From << "-" << (*it)->GetTube()->GetFrameSpan().To << "\n";
				}
				*/

				for(Model::Scenario::TubeMappingCollection::iterator it = mappedTubes.begin(); it != mappedTubes.end(); it++)
				{
					Model::Scenario::TubeMappingPtr mapping = *it;
					Model::TubeFrameCollectionPtr tubeFrames = mapping->GetTube()->GetFrames();
					ImageProcessing::VideoFilters::IntervalVideoFilterPtr tubeSequence = activeMappings->lookAhead(mapping);
										
					tubeSequence->Next();

					Frame tubeFrame = tubeSequence->GetFrame();
					FrameMask tubeMask = (tubeFrames != NULL)? (*tubeFrames)[tubeSequence->GetFrameNumber()-1]->GetData()->GetMask() : FrameMask();
					
					blender->SetFrame(frame);
					blender->setForeground(tubeFrame);
					blender->setMask(tubeMask);
					frame = blender->GetFrame();
				}

				writer->write(frame);
				onFrameCreated(frame, n, scenario->GetLength());
				//cv::imshow("output", frame);
				//cv::waitKey(10);
				activeMappings->removeInactive(n);
			}
		}
		catch(...)
		{
			if(writer != NULL)
			{
				writer->release();
				delete writer;
			}

			throw;
		}

		if(writer != NULL)
		{
			writer->release();
			delete writer;
		}
	}

public:
	SynopsisRenderer(Params params = Params()) : IRenderer(params)
	{
		blender = NULL;
		indexer = NULL;
	}

	virtual ~SynopsisRenderer()
	{
	}

	void SetFrameBlender(ImageProcessing::FrameFilters::FrameBlenderPtr blender)
	{
		CV_Assert(blender != NULL);
		this->blender = blender;
	}

	void SetScenarioIndexer(Helpers::ScenarioIndexing::IScenarioIndexerPtr indexer)
	{
		CV_Assert(indexer != NULL);
		this->indexer = indexer;
	}
};

typedef cv::Ptr<SynopsisRenderer> SynopsisRendererPtr;


}}

#endif // SYNOPSISRENDERER_H
