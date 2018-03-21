#pragma once
#ifndef MATRIXPLOTTER_H
#define MATRIXPLOTTER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include <vector>

namespace SummarizationFramework
{
namespace Presentation
{


class MatrixPlotter
{
public:
	typedef std::vector<cv::Mat> MatVector;
	typedef std::vector<cv::Scalar> ColorVector;

private:
	int rowHeight;

	void plotRow(int idx, cv::Mat & row, cv::Mat & outputImg, cv::Scalar & color)
	{		
		CV_Assert(row.cols > 1);
		
		cv::normalize(row, row, 0, rowHeight, cv::NORM_MINMAX, -1, cv::Mat());

		int top = (1+idx)*rowHeight;
		int p1y;
		int p2y = top - (int)row.at<double>(0,0);

		for(int i = 1; i < row.cols; i++)
		{
			p1y = p2y;			
			p2y = top - (int)row.at<double>(0, i);

			cv::line(outputImg, cv::Point(i-1, p1y), cv::Point(i, p2y), color, 2, 8, 0);
		}
	}

	void plotMatrix(cv::Mat & outputImage, cv::Mat & matrix, cv::Scalar & color)
	{
		for(int r = 0; r < matrix.rows; r++)
		{
			cv::Mat currentRow(matrix, cv::Rect(0, r, matrix.cols, 1));
			plotRow(r, currentRow, outputImage, color);
		}
	}

public:
	MatrixPlotter(int rowHeight = 150)
	{
		SetRowHeight(rowHeight);
	}

	cv::Mat Plot(cv::Mat & matrix, cv::Scalar color = cv::Scalar(0, 0, 0))
	{
		cv::Mat outputImage(matrix.rows*rowHeight+10, matrix.cols, CV_8UC3);
		outputImage.setTo(255);

		plotMatrix(outputImage, matrix, color);
		return outputImage;
	}

	void Plot(const std::string & outputImageFile, cv::Mat & matrix, cv::Scalar color = cv::Scalar(0, 0, 0))
	{
		cv::Mat outputImage = Plot(matrix, color);
		cv::imwrite(outputImageFile, outputImage);
	}

	cv::Mat Plot(MatVector & matrices, ColorVector & colors)
	{
		CV_Assert(matrices.size() == colors.size());

		int rows = matrices[0].rows;
		int cols = matrices[0].cols;

		cv::Mat outputImage(rows*rowHeight+10, cols, CV_8UC3);
		outputImage.setTo(255);

		for(size_t i = 0; i < matrices.size(); i++)
		{
			cv::Mat matrix = matrices[i];
			cv::Scalar color = colors[i];

			CV_Assert(matrix.cols == cols && matrix.rows == rows);

			plotMatrix(outputImage, matrix, color);
		}

		return outputImage;
	}

	void Plot(const std::string & outputImageFile, MatVector & matrices, ColorVector & colors)
	{
		cv::Mat outputImage = Plot(matrices, colors);
		cv::imwrite(outputImageFile, outputImage);
	}

	void SetRowHeight(int rowHeight)
	{
		CV_Assert(rowHeight > 0);
		this->rowHeight = rowHeight;
	}
};

typedef cv::Ptr<MatrixPlotter> MatrixPlotterPtr;


}}

#endif // MATRIXPLOTTER_H
