#pragma once
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <opencv2/core/core.hpp>

namespace SummarizationFramework
{
namespace Helpers
{


class Geometry
{
public:
	static cv::Rect RectIntersection(cv::Rect r1, cv::Rect r2)
	{
		return r1 & r2;
	}

	static cv::Point RectCenter(cv::Rect rect)
	{
		return cv::Point(rect.x + rect.width/2, rect.y + rect.height/2);
	}

	static double PointsDistance(cv::Point pt1, cv::Point pt2)
	{
		return sqrt((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y));
	}

	static double Fmeasure(double precision, double recall) {
		return (precision+recall == 0)? 0 : 2*precision*recall/(precision+recall);
	}

	static double Fmeasure(cv::Rect gt, cv::Rect eval) {
		cv::Rect intersection = RectIntersection(gt, eval);
		double intersectionArea = intersection.area();

		double recall = intersectionArea/gt.area();
		double precision = intersectionArea/eval.area();
	
		return Fmeasure(precision, recall);
	}
};


}}

#endif // GEOMETRY_H
