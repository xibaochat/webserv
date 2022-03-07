#include "webserv.hpp"

std::vector<size_t> get_occurences_end_indexes(std::string s, std::string sub)
{
    std::vector<size_t> indexes;
    size_t pos = s.find(sub, 0);
    while(pos != s.npos)
    {
        indexes.push_back(pos + sub.length());
        pos = s.find(sub, pos + sub.length());
    }
    return (indexes);
}
