// FrundRunner.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <process.h>
#include <iostream>
#include <winsock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

using namespace std;

#define PORT 55557   //The port on which to listen for incoming data
#define BUFFLEN 100  //The size of receive buffer
#define CLIENT "127.0.0.1"  //IP address of client
#define MODELS_PATH "..\\Models\\"
#define RASHET32 "\\rashet32.exe"

SOCKET sock;
struct sockaddr_in si_addr, si_other;
WSADATA wsa;

void initSocket();
void closeSocket();
bool receive(char *buffer, int size, struct sockaddr_in addr, int *slen);

int _tmain(int argc, _TCHAR* argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi1, pi2;

	char buffer[BUFFLEN];
	memset(buffer, 0, BUFFLEN);
	int slen = sizeof(si_other);

	GetStartupInfo(&si);

	initSocket();

	while (true)
	{
		//Receive command
		if (receive(buffer, 5, si_other, &slen))
			printf("Received command: %s\n", buffer);
		else
			printf("Received not enough data!\n");

		if (buffer[0] == '1')
		{
			if (!CreateProcess(MODELS_PATH"sinhron1"RASHET32, MODELS_PATH"sinhron1"RASHET32, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, MODELS_PATH"sinhron1", &si, &pi1))
				cout << "CreateProcess error " << GetLastError() << endl;

			cout << "Frund 1 must be started" << endl;
		}
		else if (buffer[0] == '2')
		{
			if (!CreateProcess(MODELS_PATH"sinhron2"RASHET32, MODELS_PATH"sinhron2"RASHET32, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, MODELS_PATH"sinhron2", &si, &pi1))
				cout << "CreateProcess error " << GetLastError() << endl;

			cout << "Frund 2 must be started" << endl;
		}
	}
	
	cout << "Trying to run frund..." << endl;
	

	//if (_spawnlp(P_NOWAIT, "D:\\__Share\\sinhron\\rashet32.exe", "D:\\__Share\\sinhron\\rashet32.exe", NULL) == -1)
	//	cout << "Spawn error" << endl;

	if (!CreateProcess("..\\Models\\sinhron1\\rashet32.exe", "..\\Models\\sinhron1\\rashet32.exe", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, "..\\Models\\sinhron1", &si, &pi1))
		cout << "CreateProcess error " << GetLastError() << endl;

	cout << "Frund 1 must be started" << endl;

	if (!CreateProcess("..\\Models\\sinhron2\\rashet32.exe", "..\\Models\\sinhron2\\rashet32.exe", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, "..\\Models\\sinhron2", &si, &pi2))
		cout << "CreateProcess error " << GetLastError() << endl;

	cout << "Frund 2 must be started" << endl;

	Sleep(5000);
	if (!TerminateProcess(pi1.hProcess, 0))
		cout << "TerminateProcess error " << GetLastError() << endl;
	if (!TerminateProcess(pi2.hProcess, 0))
		cout << "TerminateProcess error " << GetLastError() << endl;

	cout << "Frund must be killed" << endl;

	closeSocket();

	return 0;
}

void initSocket()
{
	//Initialise winsock
	printf("Initialising Winsock...\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//create socket
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
		printf("socket() failed with error code : %d\n", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//setup address structure
	memset((char*)&si_addr, 0, sizeof(si_addr));
	si_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	si_addr.sin_family = AF_INET;
	si_addr.sin_port = htons(PORT);

	printf("Binding...\n");
	if (bind(sock, (struct sockaddr*)&si_addr, sizeof(si_addr)) == SOCKET_ERROR)
	{
		printf("bind() failed with error code : %d\n", WSAGetLastError());
		closeSocket();
		exit(EXIT_FAILURE);
	}
	printf("Successfully binded.\n");

	//setup address structure
	//memset((char *)&si_other, 0, sizeof(si_other));
	//si_other.sin_family = AF_INET;
	//si_other.sin_port = htons(PORT);
	//si_other.sin_addr.S_un.S_addr = inet_addr(CLIENT);
}

void closeSocket()
{
	closesocket(sock);
	WSACleanup();
	printf("Socket is closed.\n");
}

bool receive(char *buffer, int size, struct sockaddr_in addr, int *slen)
{
	if (recvfrom(sock, buffer, size, 0, (struct sockaddr*)&addr, &*slen) != SOCKET_ERROR)
	{
		if (*slen > size)
			return false;
		return true;
	}
	else
	{
		printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
		//closeSocket();
		//exit(EXIT_FAILURE);
		return false;
	}
}

