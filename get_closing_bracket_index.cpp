#include "webserv.hpp"

int get_closing_bracket_index(std::string s, int i_start) {
    int i_end = i_start;
    int counter = 1;
    char c;

    while (counter > 0)
    {
        c = s[++i_end];
        if (c == '{')
            ++counter;
        else if (c == '}')
            --counter;
    }
    return i_end;
}
