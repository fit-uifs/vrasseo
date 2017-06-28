#pragma once
#ifndef MATRIXCSVDATASOURCE
#define MATRIXCSVDATASOURCE

#include <vector>
#include <string>
#include <fstream>
#include "../CommonTypes.h"
#include "IDataSource.h"

namespace SummarizationFramework
{
namespace Persistence
{


template<typename T>
class MatrixCsvDataSource : public IDataSource<cv::Mat>
{
private:
	typedef std::vector<T> Values;
	typedef std::vector<Values> ValuesList;
	typedef cv::Ptr<cv::Mat> MatPtr;

	std::string filename;
	char delimiter;

	// from [http://www.zedwood.com/article/cpp-csv-parser]
	void csvline_populate(std::vector<std::string> & record, const std::string & line)
	{
		int linepos=0;
	    int inquotes=false;
		char c;

		int linemax=line.length();
		std::string curstring;
		record.clear();
 
		while(line[linepos]!=0 && linepos < linemax)
		{ 
			c = line[linepos];
 
			if (!inquotes && curstring.length()==0 && c=='"')
			{
				//beginquotechar
				inquotes=true;
			}
			else if (inquotes && c=='"')
			{
				//quotechar
				if ( (linepos+1 <linemax) && (line[linepos+1]=='"') )
				{
					//encountered 2 double quotes in a row (resolves to 1 double quote)
					curstring.push_back(c);
					linepos++;
				}
				else
				{
					//endquotechar
					inquotes=false;
				}
	        }
		    else if (!inquotes && c==delimiter)
			{
				//end of field
	            record.push_back( curstring );
		        curstring="";
			}
	        else if (!inquotes && (c=='\r' || c=='\n') )
		    {
			    record.push_back( curstring );
				return;
	        }
		    else
			{
				curstring.push_back(c);
			}
			linepos++;
		}
		record.push_back( curstring );
	}

	MatPtr valuesToMatrix(ValuesList & list)
	{
		if(list.size() == 0 || list[0].size() == 0)
		{
			return new cv::Mat();
		}
		
		MatPtr result = new cv::Mat(list.size(), list[0].size(), CV_64F);

		for(size_t r = 0; r < list.size(); r++)
		{
			for(size_t c = 0; c < list[0].size(); c++)
			{
				result->at<T>(r, c) = list[r][c];
			}
		}

		return result;
	}

public:
	MatrixCsvDataSource(const std::string & filename, char delimiter = ';')
	{
		this->filename = filename;
		SetDelimiter(delimiter);
	}

	virtual MatPtr Load()
	{
		ValuesList list;

		std::ifstream fileStream(filename.c_str());
		CV_Assert(!fileStream.fail());

		std::vector<std::string> row;
		std::string line;
		int lineNo = 1;
 
		while(getline(fileStream, line) && fileStream.good())
		{
			csvline_populate(row, line);

			if(lineNo++ == 1)
			{
				for(size_t i = 0; i < row.size(); i++)
				{
					Values values;
					list.push_back(values);
				}
			}

			for(size_t i = 0; i < row.size(); i++)
			{
				T val = (T)atof(row[i].c_str()); // TODO neni moc dobre, ale neni cas vylepsovat :)
				list[i].push_back(val);
			}
		}

		fileStream.close();
		return valuesToMatrix(list);
	}

	virtual void Save(cv::Mat *entity)
	{
		cv::Mat data = *entity;
		std::ofstream file(filename.c_str());

		for(int line = 0; line < data.cols; line++)
		{
			for(int row = 0; row < data.rows; row++)
			{
				file << data.at<T>(row, line);
			
				if(row < data.rows-1)
				{
					file << delimiter;
				}
			}
			
			file << std::endl;
		}

		file.close();
	}

	void SetDelimiter(char delimiter)
	{
		this->delimiter = delimiter;
	}

	void SetFilename(const std::string & filename)
	{
		CV_Assert(!filename.empty());
		this->filename = filename;
	}
};


}}

#endif // MATRIXCSVDATASOURCE
