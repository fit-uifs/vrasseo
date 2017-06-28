#pragma once
#ifndef FRAMEWINDOW_H
#define FRAMEWINDOW_H

#include <string>
#include <map>
#include <opencv2/highgui/highgui.hpp>
#include "../CommonTypes.h"

namespace SummarizationFramework
{
namespace Presentation
{


class FrameWindow
{
private:
	std::string name;

public:
	FrameWindow(const std::string & name)
	{
		this->name = name;
	}

	~FrameWindow()
	{
		Close();
	}

	void Show(const Frame & frame)
	{
		cv::imshow(name, frame);
	}

	int ShowAndWait(const Frame & frame, int delay = 1)
	{
		cv::imshow(name, frame);
		return cv::waitKey(delay);
	}

	void Close()
	{
		cv::destroyWindow(name);
	}
};

typedef cv::Ptr<FrameWindow> FrameWindowPtr;

}}

#endif // FRAMEWINDOW_H
