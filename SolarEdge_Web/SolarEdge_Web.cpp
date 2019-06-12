// SolarEdge_Web.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock2.h>
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
int iRet;
SOCKET clientSocket;

int main()
{
#pragma region Socketlib_init

	WSADATA wsaData;														//init the WSADATA for socket lib

	iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);							//socket lib init
	if (iRet != 0)															//error catch
	{
		std::cout << "WSAStartup(MAKEWORD(2, 2), &wsaData) execute failed!";		//fatal error, end the programe
		return -1;
	}
	if (2 != LOBYTE(wsaData.wVersion) || 2 != HIBYTE(wsaData.wVersion))		//WSADATA version check
	{
		WSACleanup();														//version not right, please check the lib install
		std::cout << "WSADATA version is not correct!";
		return -1;
	}

#pragma endregion

#pragma region Socket_init

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);									//socket init
	if (clientSocket == INVALID_SOCKET)												//fatal error, end the progame
	{
		std::cout << "clientSocket = socket(AF_INET, SOCK_STREAM, 0) execute failed!";
		return -1;
	}

	/*int iMode = 1;
	iRet = ioctlsocket(clientSocket, FIONBIO, (u_long FAR*) & iMode);
	if (iRet == SOCKET_ERROR)
	{
		cout << "ioctlsocket failed!\n";
		WSACleanup();
		return -1;
	}*/

#pragma endregion

#pragma region Server_setting

	//char ip_addr[20] = "217.68.152.65";
	//struct hostent* hp = gethostbyname("www.dytt8.net");
	struct hostent* hp = gethostbyname("monitoringapi.solaredge.com");
	SOCKADDR_IN srv_Addr;									//sturcture for ip address
	memcpy(&srv_Addr.sin_addr, hp->h_addr, 4);
	srv_Addr.sin_family = AF_INET;							//stipulate the family format
	srv_Addr.sin_port = htons(80);							//define port number
	std::cout << "Connecting SolarEdge...";

#pragma endregion

#pragma region Socket_connent

	while (true)
	{
		iRet = connect(clientSocket, (SOCKADDR*)& srv_Addr, sizeof(SOCKADDR));								//try to connect
		if (0 != iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK || err == WSAEINVAL)													//connection bolcked, wait and try again
			{
				Sleep(500);
				continue;
			}
			else if (err == WSAEISCONN)
			{
				break;																						//connect successful
			}
			else
			{
				std::cout << "connect(clientSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR)) execute failed!";		//fatal error, end programe
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}

		}
	}
	std::cout << "OK\n";
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	std::cout << "\n***************************************************************";
	std::cout << "\n******************Time and Information*************************\n*******************";
	std::cout << st.wMonth << "/" << st.wDay << "/" << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond;
	std::cout << "***************************\n************";
	std::cout << "Server : " << "SolarEdge" << " Port: " << "80" << "******************\n";
	std::cout << "***************************************************************\n\n";

	int timeout = 600000;
	int m = setsockopt(clientSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)& timeout, sizeof(timeout));
	int n = setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)& timeout, sizeof(timeout));

#pragma endregion

#pragma region Request

	std::string request = "GET /site/969566/details?api_key=KR647NZKDEF2ILKO8B8OG3E50UL0IMJT HTTP/1.1\r\nHost:monitoringapi.solaredge.com\r\nConnection:Close\r\n\r\n";
	//std::string request = "GET / HTTP/1.1\r\nHost:www.dytt8.net\r\nConnection:Close\r\n\r\n";
	while (true)
	{
		iRet = send(clientSocket, request.c_str(), request.size(), 0);								//try send the data
		if (SOCKET_ERROR == iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(500);																						//send blocked, wait and try again
				continue;
			}
			else
			{
				printf("send failed!\n");																		//fatal error end programe
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}
		}
		break;
	}

#pragma endregion

#pragma region Respond

	int len = 2000;
	char* buff = new char[len];
	memset(buff, 0, len);

	int bytesRead = 0;
	int ret = 1;
	while (ret > 0) {
		ret = recv(clientSocket, buff + bytesRead, len - bytesRead, 0);
		if (ret > 0) {
			bytesRead += ret;
		}
		if (len - bytesRead < 100) {
			len = len * 2;
			char* newbuff = new char[len];
			memset(newbuff, 0, len);
			memcpy(newbuff, buff, len / 2);
			delete[] buff;
			buff = newbuff;
		}
	}
	buff[bytesRead] = '\0';
	std::cout << buff;
	

	/*char Rev_data[1000];
	while (true)
	{
		ZeroMemory(Rev_data, 1000);																				//receive data buffer
		iRet = recv(clientSocket, Rev_data, sizeof(Rev_data), 0);												//non-blocking receive
		if (SOCKET_ERROR == iRet)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);																						//receive blocked, wait and try again
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{
				printf("recv failed!\n");																		//fatal error, end programe
				closesocket(clientSocket);
				WSACleanup();
				return -1;
			}
			break;
		}
		break;
	}
	std::cout << Rev_data;
	GetLocalTime(&st);*/

#pragma endregion

	

	closesocket(clientSocket);
	WSACleanup();

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
