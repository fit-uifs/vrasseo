/*
 * TODO
 * by: Lukas Klicnar (iklicnar[at]fit.vutbr.cz)
 */

#pragma once

#include <vtapi/vtapi.h>
#include "ActivityDefs.h"
#include "SummarizationFramework/Model/IVideoSource.h"
#include "SummarizationFramework/ImageProcessing/TubesExtraction/IOutputHandler.h"

namespace Modules
{
namespace Activity
{


class VtOutputHandler : public SummarizationFramework::ImageProcessing::TubesExtraction::IOutputHandler
{
protected:
	vtapi::IntervalOutput *output;
    DetectionParams params;
	int nextGroupId;
	int commit;

	virtual void takeTube(SummarizationFramework::Model::TubePtr tube);

public:
    VtOutputHandler(vtapi::IntervalOutput *output, DetectionParams params);
	virtual ~VtOutputHandler();
};

typedef cv::Ptr<VtOutputHandler> VtOutputHandlerPtr;


}
}

