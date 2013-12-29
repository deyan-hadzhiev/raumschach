#ifndef __ERROR_H__
#define __ERROR_H__

#include <string>
#include <iostream>

class Error
{
public:
	Error( const char* msg)
		:	errmsg(msg)
	{}

	const Error& Post() const
	{
		std::cerr << errmsg << std::endl;
		return *this;
	}

	void Exit(int code) const
	{
		exit(code);
	}

private:
	std::string errmsg;
};

#endif // __ERROR_H__