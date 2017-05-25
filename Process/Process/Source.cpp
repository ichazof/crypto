#include <windows.h>
#include <tlhelp32.h>
#include <map>
#include <iostream>
#include <string>
#include <algorithm>
#include <thread>
#include <iterator>
#include <conio.h>
#include <stdlib.h>


void ProcessList(HANDLE CONST hStdOut, std::map <int, std::string> &Mmap)
{
	PROCESSENTRY32 pe32;
	HANDLE CONST hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	char tempForChar[100];
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return;
	}


	pe32.dwSize = sizeof(PROCESSENTRY32);


	Process32First(hSnapshot, &pe32);
	do
	{
		wcstombs(tempForChar, pe32.szExeFile, sizeof(pe32.szExeFile));
		std::string processName = tempForChar;
		Mmap.insert(std::pair <int, std::string>(pe32.th32ProcessID, processName));
	} while (Process32Next(hSnapshot, &pe32));



	CloseHandle(hSnapshot);
}

int main(int argc, char* argv[])
{
	std::map <int, std::string> currentProcessList;
	std::map <int, std::string> followingProcessList;

	HANDLE CONST hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	ProcessList(hStdOut, currentProcessList);
	std::map <int, std::string> differenceProcessList;

	bool flag = false;
	std::cout << "If you want to exit press any key" << std::endl;
	while (!flag)
	{
		if (_kbhit())
		{
			flag = true;
		}
		Sleep(50);
		ProcessList(hStdOut, followingProcessList);

		std::set_difference(currentProcessList.begin(), currentProcessList.end(), followingProcessList.begin(), followingProcessList.end(), std::inserter(differenceProcessList, differenceProcessList.begin()));
		if (!differenceProcessList.empty())
		{
			for (auto item : differenceProcessList)
			{
				std::cout << "Closed: " << item.first << " " << item.second << "\n";
			}
		}
		differenceProcessList.clear();

		std::set_difference(followingProcessList.begin(), followingProcessList.end(), currentProcessList.begin(), currentProcessList.end(), std::inserter(differenceProcessList, differenceProcessList.begin()));
		if (!differenceProcessList.empty())
		{
			for (auto item : differenceProcessList)
			{
				std::cout << "Opend: " << item.first << " " << item.second << "\n";
			}
		}
		differenceProcessList.clear();
		currentProcessList = followingProcessList;
		followingProcessList.clear();
	}

	return 0;
}