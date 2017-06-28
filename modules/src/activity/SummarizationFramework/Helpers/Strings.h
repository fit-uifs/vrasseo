#pragma once
#ifndef STRINGS_H
#define STRINGS_H

#include <sstream>

namespace SummarizationFramework
{
namespace Helpers
{


class Strings
{
public:
	template<typename T>
	static std::string toString(T t)
	{		
		std::stringstream ss;
		ss << t;
		return ss.str();
	}
};


}}

#endif // STRINGS_H
