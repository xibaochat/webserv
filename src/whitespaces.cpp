#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>


int is_whitespace(char c)
{
	return (c == ' ' || (c >= 9 && c <= 13));
}

void remove_fst_white_space(std::string &line)
{
	std::size_t i = 0;
	while (line[i] && is_whitespace(line[i]))
		i++;
	if (i)
		line.erase(0, i);
}

int find_first_of_whitespace(std::string &line)
{
	int i = 0;
	while (line[i] && !is_whitespace(line[i]))
		i++;
	if (i)
		return i;
	return 0;
}

int is_only_whitespace(std::string &s)
{
    for (size_t i = 0; i < s.size(); i++) {
        if (!is_whitespace(s[i]))
			return (0);
    }
	return (1);
}
