#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <conio.h>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

void user_action(); // �������� �������� � ����� ������� ����� ����
DWORD user_choice(DWORD count, HANDLE hProc[], STARTUPINFO* si, PROCESS_INFORMATION* pi); // ������ �� ����������� ��������� � �������
void edit_account(int amount, int balance); // ��������� ����� �� �������� 
void telephone_action(char* message); // ��������� �������� ������������
void close_handle(HANDLE* hPipe, HANDLE* Event, HANDLE* Semaphore, PROCESS_INFORMATION* pi); // �������� ���� ������������

int main()
{
    user_action();
    return 0;
}

class CreateError
{};

void user_action()
{
	HANDLE hPipe, Event, Semaphore, hProc[3], Synhronize_semaphore, hPID;
	DWORD dwRead, dwWritten, count=3, dwID, ExitCode;
	int flag=1;
	char* message;
	long max = 1;
	vector<DWORD> clients_name;
	STARTUPINFO si[3];
	PROCESS_INFORMATION pi[3];
	Synhronize_semaphore = CreateSemaphore(NULL, 0, max, TEXT("Queue_semaphore"));
	Event = CreateEvent(NULL, false, false, TEXT("Event")); //������� ��� ������������� ������ ���������� �������� � �����������
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\Pipe"), PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, PIPE_WAIT, 1, 64, 64, NMPWAIT_USE_DEFAULT_WAIT,NULL);
	do
	{
		flag=ReleaseSemaphore(Synhronize_semaphore, 1, NULL);
		if(flag!=0)
		{
			ConnectNamedPipe(hPipe, NULL);
			WaitForSingleObject(Event, INFINITE); //�������� ������ PID � ����
			ReadFile(hPipe, &dwID, 4, &dwRead, NULL); //������ PID
			clients_name.push_back(dwID);
			ResetEvent(Event);
			ReleaseSemaphore(Synhronize_semaphore, 1, NULL);
			DisconnectNamedPipe(hPipe);
		}
		else
		{
			for(int i=0; i<clients_name.size(); i++)
			{
				hPID = OpenProcess(PROCESS_QUERY_INFORMATION, true, clients_name[i]);
				GetExitCodeProcess(hPID, &ExitCode);
				if(ExitCode!=STILL_ACTIVE)
					clients_name.erase(clients_name.begin()+i);
				if(i==clients_name.size())
					break;
				cout<<clients_name[i]<<" ";
			}
			cout<<endl;
		}
		Sleep(500);
	}while(true);
	try
	{
		Semaphore = CreateSemaphore(NULL, 0, max, TEXT("Semaphore"));
		do
		{
			ReleaseSemaphore(Semaphore, 1, NULL); // ���������� ������� � ������ ����������� ��������, ��������� �������
			if( ConnectNamedPipe(hPipe, NULL) != FALSE)
			{
				WaitForSingleObject(Event, INFINITE); //�������� ������ �������� �� ������� ������������
				ResetEvent(Event);
				ReadFile(hPipe, message, 64, &dwRead, NULL); //������ ��������� ������������� ��������
				message[dwRead]='\0';
				system("cls");
			}
			else
			{
				cout<<"Pipe error"<<endl;
				break;
			}
			telephone_action(message); //��������� ������� ������������
			WriteFile(hPipe, message, strlen(message), &dwWritten, NULL); //�������� ������������ ���������� �������
			SetEvent(Event); // "������" ������������ �� �������� ��� ���������� �������
			count = user_choice(count, hProc, si, pi); // ����� ������������ �� ������� �������� ��� ������� � �����
			DisconnectNamedPipe(hPipe);
		}while(count!=0);
		close_handle(&hPipe, &Event, &Semaphore, pi);
	}
	catch(CreateError)
	{
		cout<<"CreateOfProcess_Error"<<endl;
		close_handle(&hPipe, &Event, &Semaphore, pi);
	}
}

void close_handle(HANDLE* hPipe, HANDLE* Event, HANDLE* Semaphore, PROCESS_INFORMATION* pi) // ��������� ��� ��������� �����������
{
	CloseHandle(hPipe);
		CloseHandle(Event);
		CloseHandle(Semaphore);
		for(int i = 0; i<3; i++)
		{
			CloseHandle(pi[i].hProcess);
			CloseHandle(pi[i].hThread);
		}
}

DWORD user_choice(DWORD count, HANDLE hProc[], STARTUPINFO* si, PROCESS_INFORMATION* pi)
{
	char temp;
	DWORD dw = WaitForMultipleObjects(count, hProc, false, INFINITE);
	cout<<endl<<"Get in line again?('y' - yes; 'n' - no)"<<endl;
	temp=getch();
	switch (dw) // ��������� ������������� �������� � ��� ����������, ���� ��������� ����������
	{
	case WAIT_OBJECT_0 + 0:
		if(temp=='y')
		{
			if(hProc[0]==pi[0].hProcess)
			{
				CreateProcess(NULL, "D:\\�����\\�����\\SPO_LB2_Client1\\Debug\\SPO_LB2_Client1.exe",NULL,NULL,false,CREATE_NEW_CONSOLE,NULL,NULL,&si[0],&pi[0]);
				hProc[0]=pi[0].hProcess;
			}
			else
			{
				if(hProc[0]==pi[1].hProcess)
				{
					CreateProcess(NULL, "D:\\�����\\�����\\SPO_LB2_Client2\\Debug\\SPO_LB2_Client2.exe",NULL,NULL,false,CREATE_NEW_CONSOLE,NULL,NULL,&si[1],&pi[1]);
					hProc[0]=pi[1].hProcess;
				}
				else
				{
					CreateProcess(NULL, "D:\\�����\\�����\\SPO_LB2_Client3\\Debug\\SPO_LB2_Client3.exe",NULL,NULL,false,CREATE_NEW_CONSOLE,NULL,NULL,&si[2],&pi[2]);
					hProc[0]=pi[2].hProcess;
				}
			}
		}
		if(temp=='n')
		{
			hProc[0]=hProc[1];
			hProc[1]=hProc[2];
			hProc[2]=0;
			count--;
		}
		break;
	case WAIT_OBJECT_0 + 1:	
		if(temp=='y')  
		{
			if(hProc[1]==pi[1].hProcess)
			{
				CreateProcess(NULL, "D:\\�����\\�����\\SPO_LB2_Client2\\Debug\\SPO_LB2_Client2.exe",NULL,NULL,false,CREATE_NEW_CONSOLE,NULL,NULL,&si[1],&pi[1]);
				hProc[1]=pi[1].hProcess;
			}
			else
			{
				CreateProcess(NULL, "D:\\�����\\�����\\SPO_LB2_Client3\\Debug\\SPO_LB2_Client3.exe",NULL,NULL,false,CREATE_NEW_CONSOLE,NULL,NULL,&si[2],&pi[2]);
				hProc[1]=pi[2].hProcess;
			}
		}
		if(temp=='n')
		{
			hProc[1]=hProc[2];
			hProc[2]=0;
			count--;
		}
		break;
	case WAIT_OBJECT_0 + 2:	
		if(temp=='y')
		{
			CreateProcess(NULL, "D:\\�����\\�����\\SPO_LB2_Client3\\Debug\\SPO_LB2_Client3.exe",NULL,NULL,false,CREATE_NEW_CONSOLE,NULL,NULL,&si[2],&pi[2]);
			hProc[2]=pi[2].hProcess;
		}
		if(temp=='n')
		{
			hProc[2]=0;
			count--;
		}
		break;
	}
	system("cls");
	return count;
}

void telephone_action(char* message)
{
	char buf[64];
	int balance=0, amount=0;
	ifstream account("D:/account.txt", ios::binary);
	account.read((char*)&balance, sizeof(balance));
	account.close();
	if(!strcmp(message,"Call")) // ��������� ������� ������������ "���������"
	{
		if(balance<40)
			cout<<"Not enough money on the account."<<endl;
		else
		{
			cout<<"Calling..."<<endl;
			amount=-40;
			edit_account(amount, balance);
		}
	}
	if(!strcmp(message, "Message")) // ��������� ������� "��������� ���������"
	{
		if(balance<50)
			cout<<"Not enough money on the account."<<endl;
		else
		{
			amount=-50;
			cout<<"Message sent"<<endl;
			edit_account(amount, balance);
		}
	}
	if(!strcmp(message, "Check account")) // �������� ��������� �����
	{
		cout<<"Account: "<<itoa(balance, buf, 10)<<" RUB"<<endl;
	}
	if(!strcmp(message, "Fund your account")) // ���������� ����� �� ��������
	{
		cout<<"Enter amount(RUB): ";
		cin>>amount;
		edit_account(amount, balance);
		cout<<"Operation completed"<<endl;
	}
}


void edit_account(int amount, int balance) // ��������� ����� �� ��������
{
	balance+=amount;
	ofstream account("D:/account.txt", ios::binary);
	account.write((char*)&balance, sizeof(balance));
	account.close();
}

