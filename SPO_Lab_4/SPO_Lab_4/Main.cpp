#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <vector>
#include <conio.h>
#define MAX_THREAD 6
using namespace std;

int return_number(char * string);
DWORD WINAPI create_thread(LPVOID par);
HANDLE* thread_for_new_thread();
DWORD WINAPI print_thread_name(LPVOID t);
DWORD WINAPI print_threads(LPVOID par);
vector<HANDLE> hThreads;
DWORD time_to_new;
DWORD time_out;

int main(int argc, char* argv[])
{
	char temp ='0';
	HANDLE* mThreads;
	if(argc <3)
	{
		cout<<"Not enough arguments"<<endl;
		return 0;
	}
	else
	{
		time_to_new = return_number(argv[1]);
		time_out = return_number(argv[2]);
	}
	cout<<"Create new thread: 'C'"<<endl<<"Delete thread: 'D'"<<endl;
	mThreads = thread_for_new_thread();
	while(1)
	{
		temp = getch();
		if(temp == 'c')

			hThreads.push_back(CreateThread(0, 0, print_thread_name, 0, CREATE_SUSPENDED, 0));
		if(temp == 'd')
		{
			TerminateThread(hThreads[0], 0);
			hThreads.erase(hThreads.begin());
		}
		if(temp == 'e')
		{
			for(int i=0; i<hThreads.size(); i++)
			{
				if(i<2)
					TerminateThread(mThreads[i], 0);
				TerminateThread(hThreads[i], 0);
			}
			return 0;
		}
	}
}

/////—оздание двух потоков, один - создает новые потоки автоматически, а второй контролирует очередность их вывода на экран////
HANDLE* thread_for_new_thread() 
{
	HANDLE mainThreads [2];
	mainThreads[0] = CreateThread(0, 0, create_thread, 0, 0, 0);
	mainThreads[1] = CreateThread(0, 0, print_threads, 0, 0, 0);
	return mainThreads;
}

/////ѕроход по очереди потоков и разрешение вывода их имени/////
DWORD WINAPI print_threads(LPVOID par)
{
	while(1)
	{
		for(int i=0; i<hThreads.size(); i++)
		{
			ResumeThread(hThreads[i]);
			Sleep(time_out);
		}
		cout<<endl;
	}
}

/////—оздание потоков, вывод€щих свое им€ на экран с определенным интервалом///////
DWORD WINAPI create_thread(LPVOID par) 
{
	while(1)
	{
		if(hThreads.size() >= MAX_THREAD)
			continue;
		hThreads.push_back(CreateThread(0, 0, print_thread_name, 0, CREATE_SUSPENDED, 0));
		Sleep(time_to_new);
	}
}

//////¬ывод имени потока на экран/////
DWORD WINAPI print_thread_name(LPVOID t)
{
	while(1)
	{
		cout<<GetCurrentThreadId()<<"  ";
		SuspendThread(GetCurrentThread());
	}
}

int return_number(char* string)
{
	char buffer[10];
	strcpy(buffer, string);
	return atoi(buffer);
}