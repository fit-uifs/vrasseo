#pragma once
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

namespace SummarizationFramework
{


class SummarizationFrameworkException// : public std::exception
{
private:
	std::string message;

public:
	SummarizationFrameworkException()// : std::exception()
	{
	}

	SummarizationFrameworkException(const char *message)// : std::exception()
	{
		this->message = message;
	}

	SummarizationFrameworkException(std::string message)// : std::exception()
	{
		this->message = message;
	}

	std::string GetMessage() const
	{
		return message;
	}
};

class NotSupportedException : public SummarizationFrameworkException
{
public:
	NotSupportedException() : SummarizationFrameworkException()
	{
	}

	NotSupportedException(const char *message) : SummarizationFrameworkException(message)
	{
	}

	NotSupportedException(std::string message) : SummarizationFrameworkException(message.c_str())
	{
	}
};

class NotImplementedException : public SummarizationFrameworkException
{
public:
	NotImplementedException() : SummarizationFrameworkException()
	{
	}

	NotImplementedException(const char *message) : SummarizationFrameworkException(message)
	{
	}

	NotImplementedException(std::string message) : SummarizationFrameworkException(message.c_str())
	{
	}
};

class ArgumentOutOfRangeException : public SummarizationFrameworkException
{
public:
	ArgumentOutOfRangeException() : SummarizationFrameworkException()
	{
	}

	ArgumentOutOfRangeException(const char *message) : SummarizationFrameworkException(message)
	{
	}

	ArgumentOutOfRangeException(std::string message) : SummarizationFrameworkException(message.c_str())
	{
	}
};

}

#endif // EXCEPTIONS_H
