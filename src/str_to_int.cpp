#include "webserv.hpp"

int cast_as_int(std::string s)
{
	std::stringstream ss;
	int ret;

	ss << s;
	ss >> ret;
	return (ret);
}
