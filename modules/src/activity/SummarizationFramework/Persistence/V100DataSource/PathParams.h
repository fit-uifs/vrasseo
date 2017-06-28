#pragma once
#ifndef V100DATASOURCE_PATHPARAMS_H
#define V100DATASOURCE_PATHPARAMS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "../../Helpers/Strings.h"

namespace SummarizationFramework
{
namespace Persistence
{
namespace V100DataSource
{


class PathParams
{
protected:
	std::string baseDir;
	std::string name;
	
	std::string tubesXmlFile;
	
	std::string tubeDataXmlFilePrefix;
	std::string tubeDataXmlFileSuffix;

	std::string tubeMaskFilePrefix;
	std::string tubeMaskFileSuffix;

	int maskFourCc;
	double maskFps;

	bool empty;

public:
	PathParams()
	{
		empty = true;
	}

	PathParams(std::string baseDir, std::string name = "video")
	{
		tubeDataXmlFileSuffix = ".data.xml";
		tubeMaskFileSuffix = ".mask.avi";
		maskFourCc = CV_FOURCC('D','I','B',' '); // TODO
		maskFps = 25; // TODO
		this->name = name;

		SetBaseDir(baseDir);
	}

	void SetBaseDir(std::string dir)
	{
		CV_Assert(!dir.empty());

		baseDir = dir;
		tubesXmlFile = baseDir + "\\" + name + ".tubes.xml";
		tubeDataXmlFilePrefix = baseDir + "\\" + name + ".tube";
		tubeMaskFilePrefix = tubeDataXmlFilePrefix;

		empty = false;
	}

	std::string GetTubesXmlFilePath() const
	{
		// video.tubes.xml
		CV_Assert(!empty);
		return tubesXmlFile;
	}

	std::string GetTubeDataXmlFilePath(int tubeId) const
	{
		// video.tubeN.data.xml
		CV_Assert(!empty);
		return tubeDataXmlFilePrefix + Helpers::Strings::toString(tubeId) + tubeDataXmlFileSuffix;
	}

	std::string GetTubeMaskFilePath(int tubeId) const
	{
		// video.tubeN.mask.avi
		CV_Assert(!empty);
		return tubeMaskFilePrefix + Helpers::Strings::toString(tubeId) + tubeMaskFileSuffix;
	}

	int GetMaskFourCc() const
	{
		CV_Assert(!empty);
		return maskFourCc;
	}

	double GetMaskFps() const
	{
		CV_Assert(!empty);
		return maskFps;
	}

	bool IsEmpty() const
	{
		return empty;
	}
};


}}}

#endif // V100DATASOURCE_PATHPARAMS_H
