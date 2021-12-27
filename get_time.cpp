#include "webserv.hpp"

void get_time(std::string &response)
{
	std::time_t result = std::time(NULL);
    std::string time = std::asctime(std::localtime(&result));
    std::size_t found = time.find_last_of("\n");
    time = time.substr(0,found);

    std::stringstream ss_time(time);
    std::string segment;
    std::vector<std::string> seglist;

    int i = 0;
    while(std::getline(ss_time, segment, ' '))
    {
        if (!i)
            segment.append(",");
        segment.append(" ");
        seglist.push_back(segment); //Spit string at '_' character
        i++;
    }
    std::string date;
    date.append(seglist[0]);
    date.append(seglist[2]);
    date.append(seglist[1]);
    date.append(seglist[4]);
    date.append(seglist[3]);
    date.append("GMT");
    response.append(date);
}
