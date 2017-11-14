#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <conio.h>
using namespace std;

int telephone_function() // выбор пользователем функции
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
		cout<<"Select an action(CL1):"<<endl;
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
	DWORD dwRead, dwWritten, process_id;
	HANDLE hPipe, Event, Semaphore, Synhronize_semaphore;
	char send_str[64];
	char st[64];
	process_id = GetCurrentProcessId();
	Event = OpenEvent(EVENT_ALL_ACCESS, TRUE, TEXT("Event"));
	Synhronize_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, TEXT("Queue_semaphore"));
	WaitForSingleObject(Synhronize_semaphore, INFINITE);
	WaitNamedPipe(TEXT("\\\\.\\pipe\\Pipe"), INFINITE); // ожидание готовности "пайпа" к соединению с даным процессом
    hPipe = CreateFile(TEXT("\\\\.\\pipe\\Pipe"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); // соединение с пайпом
	strcpy(send_str,"Client1");
	WriteFile(hPipe, &process_id, strlen(send_str), &dwWritten, NULL);
	SetEvent(Event);
	DisconnectNamedPipe(hPipe);
	Semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, TEXT("Semaphore"));
	cout<<"Phone is currently unavailable"<<endl;
	WaitForSingleObject(Semaphore, INFINITE); // ожидание разрешения на работу пользователя с телефоном
	system("cls");
	switch(telephone_function())
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
	WriteFile(hPipe, send_str, strlen(send_str), &dwWritten, NULL); //передача выбранного пользователем действия
	SetEvent(Event); //сигнал процессу "телефон" о возможности обработки действия пользовтеля
	WaitForSingleObject(Event, INFINITE); // ожидание ответа от "телефона"
	ReadFile(hPipe, st, 64, &dwRead, NULL);
	st[dwRead]='\0';
	cout<<st<<endl;
	CloseHandle(Semaphore);
    CloseHandle(hPipe);
	CloseHandle(Event);
    return 0;
}
