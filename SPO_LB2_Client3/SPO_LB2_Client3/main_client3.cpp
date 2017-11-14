#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <conio.h>
using namespace std;

int teleph_function()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position;
	position.X=0;
	position.Y=1;
	int temp;
	SetConsoleCursorPosition(hConsole,position);
	while(1)
	{
		system ("cls");
		cout<<"Select an action(CL3):"<<endl;
		cout<<"  1. Call(40 RUB)"<<endl<<"  2. Send a message(50 RUB)"<<endl;
		cout<<"  3. Check account"<<endl<<"  4. Fund your account"<<endl;
		SetConsoleCursorPosition(hConsole,position);
		cout<<"* ";
		temp=getch();
		switch(temp)
		{
		case 80:
			position.Y++;
			if(position.Y>5)
				position.Y--;
			break;
		case 72:
			position.Y--;
			if(position.Y<1)
				position.Y++;
			break;
		case 13:
			return position.Y;
		}
	}
}
int main()
{
	DWORD dwRead, dwWritten;
	HANDLE hPipe, Event, Semaphore;
	char send_str[64];
	char st[64];
	Event = OpenEvent(EVENT_ALL_ACCESS, TRUE, TEXT("Event"));
	Semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, TEXT("Semaphore"));
	cout<<"Phone is currently unavailable"<<endl;
	WaitForSingleObject(Semaphore, INFINITE);
	system("cls");
	WaitNamedPipe(TEXT("\\\\.\\pipe\\Pipe"), INFINITE);
    hPipe = CreateFile(TEXT("\\\\.\\pipe\\Pipe"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	switch(teleph_function())
	{
	case 1:
		strcpy(send_str,"Call");
		break;
	case 2:
		strcpy(send_str,"Message");
		break;
	case 3:
		strcpy(send_str,"Check account");
		break;
	case 4:
		strcpy(send_str, "Fund your account");
		break;
	}
	WriteFile(hPipe, send_str, strlen(send_str), &dwWritten, NULL);
	SetEvent(Event);
	WaitForSingleObject(Event, INFINITE);
	ReadFile(hPipe, st, 64, &dwRead, NULL);
	st[dwRead]='\0';
	cout<<st<<endl;
	CloseHandle(Semaphore);
    CloseHandle(hPipe);
	CloseHandle(Event);
    return 0;
}