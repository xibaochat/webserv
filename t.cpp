#include <fstream>
#include <iostream>
using namespace std;

int main()
{
    ofstream myFile_Handler;

    // File Open
    myFile_Handler.open("File_1.txt");
	if (myFile_Handler.is_open())
		std::cout << "YO\n";
	else
		cout << "NO\n";
    // File Close
    myFile_Handler.close();
    return 0;
}
