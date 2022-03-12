#include "webserv.hpp"

std::string GetFileSize(std::string filename)
{
	DIR *dir;
	dir = opendir(filename.c_str());
	if (dir != NULL)
	{
		closedir (dir);
		return "-";
	}
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
	closedir (dir);
	std::stringstream stream;
	if (rc == 0)
	{
		stream << stat_buf.st_size;
		return stream.str();
	}
	stream << 0;
	return stream.str();
}

std::vector<std::string> seperate_string(std::string s)
{
	std::string space_delimiter = " ";
    std::vector<std::string> words;

    size_t pos = 0;
    while ((pos = s.find(space_delimiter)) != std::string::npos)
	{
		if (s.substr(0, pos).length() != 0)
			words.push_back(s.substr(0, pos));
		s.erase(0, pos + space_delimiter.length());
    }
	space_delimiter = "\n";
	if ((pos = s.find(space_delimiter)) != std::string::npos)
		words.push_back(s.substr(0, pos));
	s.erase(0, pos + space_delimiter.length());
	return words;
}

std::string getFileCreationTime(std::string path)
{
    struct stat attr;
    stat(path.c_str(), &attr);
	std::vector<std::string> v = seperate_string(ctime(&attr.st_mtime));
	std::string time(v[2] + "-" + v[1] + "-" + v[4] + " " + v[3]);
	return time;
}

std::string get_file_output(Client_Request &obj)
{
	std::string o_path = obj.clean_relative_path;
	std::string path = obj.file;
	DIR *dir;
	std::string dir_output;//we need to show directory first in auto_index list
	std::string file_output;
	struct dirent *ent;
	std::string file_whole_path;
	std::string index_output("<html>\n<head><title>Index of " + path + "</title></head>\n<body>\n<h1>Index of " + path + "</h1><hr><pre><a href=\"../\">../</a>\n");

	if ((dir = opendir (path.c_str())) != NULL)//can open the directory
	{
		while ((ent = readdir (dir)) != NULL)
		{
			if (ent->d_name[0] && ent->d_name[0] != '.')
			{
				file_whole_path = path + "/" + ent->d_name;//whole path
				std::string file_link(ent->d_name);//href link
					if (path.back() != '/')
						file_link = path + "/" + file_link;
					else
						file_link = "./" + file_link;
				std::string time = getFileCreationTime(file_whole_path);//time
				std::string file_size = GetFileSize(file_whole_path);//size
				std::string full_d_name = ent->d_name;
				std::string d_name = ent->d_name;

				if (file_size == "-")//it is a dir
				{
					full_d_name += "/";
					d_name += "/";
				}
				if (d_name.length() >= 51) // limit to 51 to format names as nginx
				{
					d_name = d_name.substr(0, 50);
					d_name[49] = '>';
					d_name[48] = '.';
					d_name[47] = '.';
				}
				std::string spaces(51 - d_name.length(), ' ');
				std::string space_filler(20 - file_size.length(), ' ');
				std::string str = "<a href=\"" + file_link + "\">" + d_name + "</a> " + spaces + time + space_filler + file_size + "\n";
				if (file_size == "-")
					dir_output += str;
				else
					file_output += str;
			}
		}
		index_output += dir_output + file_output;
		index_output += "</pre><hr></body>\n</html>\n";
		closedir (dir);
	}
	else
		throw ("[ERROR]Cannot open the file");
	return index_output;
}
