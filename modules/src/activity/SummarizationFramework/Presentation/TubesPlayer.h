#pragma once
#ifndef TUBESPLAYER_H
#define TUBESPLAYER_H

#include "../Model/TubeCollection.h"
#include <opencv2/highgui/highgui.hpp>

namespace SummarizationFramework
{
namespace Presentation
{


class TubesPlayer
{
public:
	static void Play(Model::TubeCollectionPtr tubes)
	{
		for(Model::TubeCollection::Iterator it = tubes->Begin(); it != tubes->End(); it++)
		{
			Model::TubeFrameCollectionPtr frames = it->obj->GetFrames();

			for(Model::TubeFrameCollection::iterator it2 = frames->begin(); it2 != frames->end(); it2++)
			{
				FrameMask mask = it2->obj->GetData()->GetMask();
				imshow("tube", mask);
				cv::waitKey(1);
			}
		}
	}
};


}}

#endif // TUBESPLAYER_H
