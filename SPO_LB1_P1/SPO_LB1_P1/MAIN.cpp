#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <conio.h>
#include <string>
#include <fstream>
using namespace std;

void user_action();
int teleph_function();

int main()
{
    user_action();
    return 0;
}

void edit_account(int amount, int balance)
{
	balance+=amount;
	ofstream account("D:/account.txt", ios::binary);
	account.write((char*)&balance, sizeof(balance));
	account.close();
}

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
		cout<<"Select an action:"<<endl;
		cout<<"  1. Call(40 RUB)"<<endl<<"  2. Send a message(50 RUB)"<<endl;
		cout<<"  3. Check account"<<endl<<"  4. Fund your account"<<endl<<"  5. Exit"<<endl;
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

void user_action()
{
	HANDLE hPipe, Event, Semaphore;
	DWORD dwRead, dwWritten;
	int balance=0, amount=0, max=1;
	char message[64] , buf[64];
	bool flag=false;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe"),
								PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
								PIPE_WAIT,
								1,
								64,
								64,
								NMPWAIT_USE_DEFAULT_WAIT,
								NULL);
	ZeroMemory(&si, sizeof(si));
	si.cb=sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	Event = CreateEvent(NULL, false, false, TEXT("Event"));
	Semaphore = CreateSemaphore(NULL, 0, max, TEXT("Semaphore"));
	do
	{
		CreateProcess(NULL, "D:\\Kyrsach\\Test\\Debug\\Client.exe",NULL,NULL,false,NULL,NULL,NULL,&si,&pi);
		ReleaseSemaphore(Semaphore, 1, NULL);
			if( ConnectNamedPipe(hPipe, NULL) != FALSE)
			{
				WaitForSingleObject(Event, INFINITE);
				ResetEvent(Event);
				ReadFile(hPipe, message, 64, &dwRead, NULL);
				message[dwRead]='\0';
				system("cls");
				ifstream account("D:/account.txt", ios::binary);
				account.read((char*)&balance, sizeof(balance));
				account.close();
			}
			else
			{
				cout<<"Pipe error"<<endl;
				break;;
			}
			if(!strcmp(message,"Call"))
			{
				if(balance<40)
					strcpy(message,"Not enough money on the account.");
				else
				{
					strcpy(message, "Calling...");
					amount=-40;
					edit_account(amount, balance);
				}
			}
			if(!strcmp(message, "Message"))
			{
				if(balance<50)
					strcpy(message, "Not enough money on the account.");
				else
				{
					amount=-50;
					strcpy(message, "Message sent");
					edit_account(amount, balance);
				}
			}
			if(!strcmp(message, "Check account"))
			{
				strcpy(message, "Account: ");
				strcat(message, itoa(balance, buf, 10));
				strcat(message, " RUB");
			}
			if(!strcmp(message, "Fund your account"))
			{
				cout<<"Enter amount(RUB): ";
				cin>>amount;
				edit_account(amount, balance);
				strcpy(message, "Operation completed");
			}
			if(!strcmp(message, "Close"))
				flag=true;
				WriteFile(hPipe, message, strlen(message), &dwWritten, NULL);
				SetEvent(Event);
				WaitForSingleObject(pi.hProcess, INFINITE);
				DisconnectNamedPipe(hPipe);
			}while(!flag);
	CloseHandle(Event);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hPipe);
}

