#include "webserv.hpp"

int get_closing_bracket_index(std::string s, int i_start) {
    int i_end = i_start;
    int counter = 1;
    char c;

    while (s[i_end] && counter > 0)
    {
        c = s[i_end++];
        if (c == '{')
            ++counter;
        else if (c == '}')
            --counter;
    }
	if (counter > 0)
		return (-1);
    return i_end;
}
