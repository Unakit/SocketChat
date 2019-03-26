//Client

#include "stdfix.h"
#define _WIN32_WINNT 0x0501
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <windows.h>
#include <SDKDDKVer.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <wincon.h>
#include <stdio.h>

#pragma warning(disable: 4996)

SOCKET Connection;
bool isServerFeelsGood;

void ClientHandler(bool *a) {
	int messageSize;
	while(true) {
		if((recv(Connection, (char*)&messageSize, sizeof(int), NULL))>0){
			char *message = new char[messageSize + 1];
			message[messageSize] = '\0';
			recv(Connection, message, messageSize, NULL);
			std::cout << message << std::endl;
			delete[] message;
		}else break;
	}
	closesocket(Connection);
	WSACleanup();
	//std::cout<<"You are disconnected from the chat. To restart conversation enter '/restart'"<<std::endl;
	isServerFeelsGood=false;
	return void();
	ExitThread(0);
}

/*BOOL WINAPI HandlerRoutine(DWORD eventCode){
	switch(eventCode){
		case CTRL_CLOSE_EVENT:
			closesocket(Connection);
			return FALSE;
			break;
	}
	return TRUE;
}*/

int main(int argc, char* argv[]) {
	//SetConsoleCtrlHandler(HandlerRoutine, TRUE);
	std::cout<<"To start chatting send '/start' command, to close this app send '/exit' command ";
	const int port=1111;
	std::string cmd, login;
	do{
	std::getline(std::cin, cmd);
	if(cmd=="/exit"){
		std::cout<<"Hope to see you soon\n";
		system("pause");
		return 0;
	}
	}while(cmd!="/start");
	std::cout<<"To take part in chat, enter your name: ";
	std::getline(std::cin, login);
while(1){
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if(WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Something goes wrong, please check your internet connection..." << std::endl;
		return 0;
	}
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	
	if(connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		std::cout << "Error: can't connect.\n";
		Sleep(1000);
	}else{
		int logSize=login.size();
		send(Connection, (char*)&logSize, sizeof(int), NULL);
		send(Connection, login.c_str(), logSize, NULL);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
		std::string message;
		std::string outMsg;
		isServerFeelsGood=true;
		while(isServerFeelsGood) {
			std::getline(std::cin, message);
			if(message=="/h" || message=="/online"){
				int outMsg_size = message.size();
				send(Connection, (char*)&outMsg_size, sizeof(int), NULL);
				send(Connection, message.c_str(), outMsg_size, NULL);
			}else if(message=="/exit"){
				std::cout<<"Hope to see you soon\n";
				closesocket(Connection);
				WSACleanup;
				system("pause");
				return 0;
			}else{
				outMsg=login+": "+message;
				int outMsg_size = outMsg.size();
				send(Connection, (char*)&outMsg_size, sizeof(int), NULL);
				send(Connection, outMsg.c_str(), outMsg_size, NULL);
				Sleep(10);
		}
		}
	}
	std::cout<<"Something goes wrong. Do you want to try again?\nSend 'yes', if you want, and 'no' if you don't': ";
	while(1){
		std::getline(std::cin, cmd);
		if(cmd=="yes"){
			break;
		}else if(cmd=="no"){
			std::cout<<"Hope to see you soon\n";
			system("pause");
			return 0;
		}else{
			std::cout<<"Unknown command...\nSend 'yes' or 'not' command, and nothing else... ";
		}
	}
}
	closesocket(Connection);
	WSACleanup;
	system("pause");
	return 0;
}
