#include "webserv.hpp"

std::vector<std::string> extract_words_in_vector(std::string &s)
{
	std::vector<std::string>v;
	std::string elem;
    int i = 0;
    while (s[i])
    {
        while (s[i] && s[i] == '/')
            i++;
        while (s[i] && s[i] != '/')
            elem += s[i++];
        if (elem.length() > 0)
            v.push_back(elem);
        elem.clear();
    }
	return v;
}

int check_substring(std::string s, std::string s1)
{
    std::vector<std::string>v1,v2;
    v1 = extract_words_in_vector(s);
	v2 = extract_words_in_vector(s1);
	std::vector<std::string>::iterator it0 = v1.begin();
	if (v1.size() >= v2.size())
    {
        for (std::vector<std::string>::iterator it = v2.begin() ; it != v2.end(); ++it)
		{
			if (*it0 != *it)
				return 0;
			it0++;
		}
        return 1;
    }
    return 0;
}

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
