#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27016
#define SERVER_SLEEP_TIME 50
#define MAX_SIZE 1000

// Initializes WinSock2 library
// Returns true if succeeded, false otherwise.
bool InitializeWindowsSockets();
void CheckSend(SOCKET connectSocket);
bool Send(SOCKET connectSocket, char *messageToSend, int size);

int __cdecl main(int argc, char **argv) 
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    // message to send
    
    
    // Validate the parameters
    if (argc != 2)
    {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    if(InitializeWindowsSockets() == false)
    {
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
    }

    // create a socket
    connectSocket = socket(AF_INET,
                           SOCK_STREAM,
                           IPPROTO_TCP);

    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // create and initialize address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddress.sin_port = htons(DEFAULT_PORT);
    // connect to server specified in serverAddress and socket connectSocket
    if (connect(connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Unable to connect to server.\n");
        closesocket(connectSocket);
        WSACleanup();
    }
 


	while (1)
	{
		char buffer[MAX_SIZE];
		printf("Unesite poruku koju hocete da posaljete: ");
		gets_s(buffer);
		int ab = 100 * 1024 * 1024;
		char *niz = (char*)malloc(ab);
		for (int i = 0; i < ab; i++)
		{
			*(niz+i) = i % 127;
		}




		CheckSend(connectSocket);
		char *messageToSend = (char*)malloc(4); //duzina poruka koja bi se trebala poslati
		*((int*)(messageToSend)) =ab;
		



		if (!Send(connectSocket, messageToSend, 4))
		{
			printf("Greska pri slanju");
		}


		


		if (!Send(connectSocket, niz, ab))
		{
			printf("Greska pri slanju");
		}
		

	
		
	}
    
	getchar();
    // cleanup
    closesocket(connectSocket);
    WSACleanup();

    return 0;
}

bool InitializeWindowsSockets()
{
    WSADATA wsaData;
	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return false;
    }
	return true;
}


void CheckSend(SOCKET connectSocket)
{
	while (1)
	{
		FD_SET set;
		timeval timeVal;

		FD_ZERO(&set);
		// Add socket we will wait to read from
		FD_SET(connectSocket, &set);

		// Set timeouts to zero since we want select to return
		// instantaneously
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;
		int iResult;
		iResult = select(0 ,NULL, &set, NULL, &timeVal);

		// lets check if there was an error during select
		if (iResult == SOCKET_ERROR)
		{
			fprintf(stderr, "select failed with error: %ld\n", WSAGetLastError());
			continue;
		}

		// now, lets check if there are any sockets ready
		if (iResult == 0)
		{
			// there are no ready sockets, sleep for a while and check again
			Sleep(SERVER_SLEEP_TIME);
			continue;
		}
		break;
	}
}
bool Send(SOCKET connectSocket, char *messageToSend, int size)
{
	int toSend = 0;
	while (1)
	{
		int iResult=0;
		CheckSend(connectSocket);
		iResult = send(connectSocket, messageToSend+iResult, size-toSend, 0);
		toSend = toSend + iResult;
		printf("Bytes Sent: %ld\n", iResult);
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return false;
		}
		if (iResult == 0)
		{
			break;
		}
		if (toSend == size)
		{
			break;
		}
		
	}
	return true;
}