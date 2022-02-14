#include "webserv.hpp"

int count_words(std::string str)
{
    int count = 0;
	int i = 0;
    for (i = 0; str[i] != '\0';i++)
    {
        while (str[i] && str[i] == ' ')
            i++;
        if (str[i] && str[i] != ' ')
        {
            count++;
            i++;
        }
        while (str[i] && str[i] != ' ')
            i++;
    }
	return count;
}
