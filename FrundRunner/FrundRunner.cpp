// FrundRunner.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <process.h>
#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <Windows.h>
#include <fstream>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

using namespace std;

#define PORT 55557   //The port on which to listen for incoming data
#define BUFFLEN 3  //The size of receive buffer
#define CLIENT "127.0.0.1"  //IP address of client
#define MODELS_PATH "..\\Models\\"
#define RASHET32 "\\rashet32.exe"
#define CONTROL_FILE "\\CONTROLFILE.TXT"

SOCKET sock;
struct sockaddr_in si_addr, si_other;
WSADATA wsa;

void initSocket();
void closeSocket();
bool receive(char *buffer, int size, struct sockaddr_in *addr, int *slen);
bool send(char *buffer, int size, struct sockaddr_in *addr, int slen);

int _tmain(int argc, _TCHAR* argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	char buffer[BUFFLEN];
	char model_buffer[20];
	char params_buffer[150];
	memset(buffer, 0, BUFFLEN);
	memset(model_buffer, 0, 20);
	memset(params_buffer, 0, 150);
	int slen = sizeof(si_other);

	GetStartupInfo(&si);

	initSocket();

	while (true)
	{
		//Receive command
		if (receive(buffer, 10, &si_other, &slen))
		{
			printf("Received command: %s\n", buffer);

			if (buffer[0] == '1')	// run model
			{
				if (receive(model_buffer, 20, &si_other, &slen))
				{
					printf("Model name received: %s\n", model_buffer);


					char model[50] = { 0 }, model_exe[100] = { 0 };
					strcat(strcat(model, MODELS_PATH), model_buffer);
					strcat(strcat(model_exe, model), RASHET32);

					if (!CreateProcess(model_exe, model_exe, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, model, &si, &pi))
					{
						cout << "CreateProcess error " << GetLastError() << endl;
						buffer[0] = '2';
					}
					else
						buffer[0] = '1';
					buffer[1] = 0;
					send(buffer, 2, &si_other, slen);
				}

				memset(model_buffer, 0, 20);
			}
			else if (buffer[0] == '2')	// stop model
			{
				if (!TerminateProcess(pi.hProcess, 0))
				{
					cout << "TerminateProcess error " << GetLastError() << endl;
					buffer[0] = '2';
				}
				else
					buffer[0] = '1';
				buffer[1] = 0;
				send(buffer, 2, &si_other, slen);
			}
			else if (buffer[0] == '3')	// write params
			{
				if (receive(model_buffer, 20, &si_other, &slen))
				{
					printf("Model name received: %s\n", model_buffer);

					char model[50] = { 0 }, model_params[100] = { 0 };
					strcat(strcat(model, MODELS_PATH), model_buffer);
					strcat(strcat(model_params, model), CONTROL_FILE);

					if (receive(params_buffer, 100, &si_other, &slen))
					{
						//write to file
						ofstream file;
						file.open(model_params, ios_base::out);
						if (!file.is_open())
						{
							printf("Open file error!\n");
							buffer[0] = '2';
						}
						else
						{
							file << params_buffer;
							buffer[0] = '1';
						}
						file.close();
						buffer[1] = '0';
						send(buffer, 2, &si_other, slen);
					}

					memset(params_buffer, 0, 150);
				}

				memset(model_buffer, 0, 20);
			}

			memset(buffer, 0, BUFFLEN);
		}
	}

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

bool receive(char *buffer, int size, struct sockaddr_in *addr, int *slen)
{
	if (recvfrom(sock, buffer, size, 0, (struct sockaddr*)addr, &*slen) != SOCKET_ERROR)
		return true;
	printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
	//closeSocket();
	//exit(EXIT_FAILURE);
	return false;
}

bool send(char *buffer, int size, struct sockaddr_in *addr, int slen)
{
	if (sendto(sock, buffer, size, 0, (struct sockaddr*)addr, slen) != SOCKET_ERROR)
		return true;
	printf("sendto() failed with error code : %d\n", WSAGetLastError());
	//closeSocket();
	//exit(EXIT_FAILURE);
	return false;
}

