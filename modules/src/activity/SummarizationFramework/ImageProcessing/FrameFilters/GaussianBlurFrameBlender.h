#pragma once
#ifndef GAUSSIANBLURFRAMEBLENDER_H
#define GAUSSIANBLURFRAMEBLENDER_H

#include <opencv2/imgproc/imgproc.hpp>
#include "../../Presentation/FrameWindow.h"
#include "FrameBlender.h"

namespace SummarizationFramework
{
namespace ImageProcessing
{
namespace FrameFilters
{


class SimpleFrameBlender : public FrameBlender
{
private:
	// from https://gist.github.com/Brick85/5009046
	void blendWithMask(cv::Mat &base, cv::Mat &src, cv::Mat &mask, cv::Mat &out){
		char ch = base.channels();
		double alpha = 0;
		for( int y = 0; y < base.rows; y++ ){
			uchar* pBS = base.ptr<uchar>(y);
			uchar* pSR = src.ptr<uchar>(y);
			uchar* pMK = mask.ptr<uchar>(y);
			uchar* pOU = out.ptr<uchar>(y);
			for( int x = 0; x < base.cols*ch; x++ ){
				int ix = x / ch;
				if(pMK[ix] == 0){
					pOU[x] = pBS[x];
				} else if(pMK[ix] == 255){
					pOU[x] = pSR[x];
				} else {
					alpha = pMK[ix] / 255.0;
					pOU[x] = pSR[x] * alpha + pBS[x] * (1-alpha);
				}
			}
		}
	}

	virtual void blend(Frame & background, Frame & foreground, FrameMask & mask)
	{
		FrameMask maskBlurred;	
		cv::GaussianBlur(mask, maskBlurred, cv::Size(31, 31), 10.0, 10.0);
		//foreground.copyTo(background, maskBlurred);

		cv::Mat base = background.clone();
		blendWithMask(base, foreground, maskBlurred, background);

		//Presentation::FrameWindow aaaaaaaa("pokus");
		//aaaaaaaa.ShowAndWait(out, 0);

		//Presentation::FrameWindow windowMask("mask");
		//windowMask.Show(mask);
		//Presentation::FrameWindow windowMaskBlurred("mask blurred");
		//windowMask.ShowAndWait(maskBlurred, 0);
	}

public:
	SimpleFrameBlender() : FrameBlender()
	{
	}

	SimpleFrameBlender(FrameFilterPtr base, Frame & foreground, FrameMask & mask) : FrameBlender(base, foreground, mask)
	{
	}

	SimpleFrameBlender(Frame & base, Frame & foreground, FrameMask & mask) : FrameBlender(base, foreground, mask)
	{
	}

	virtual ~SimpleFrameBlender()
	{
	}
};


typedef cv::Ptr<SimpleFrameBlender> SimpleFrameBlenderPtr;


}}}

#endif // GAUSSIANBLURFRAMEBLENDER_H
