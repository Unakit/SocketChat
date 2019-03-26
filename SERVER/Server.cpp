//Server

#define _CRT_SECURE_NO_WARNINGS
#include "stdfix.h"
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>

#pragma warning(disable: 4996)
const int maxCon=50, st=100;
SOCKET Connections[maxCon];
std::string users[maxCon];
int Counter = 0;

void timeHandler(){
	while(true){
		char t[st]="\0";
		char t1[st]="\0";
		time_t seconds=time(NULL);
		tm* timeinfo=localtime(&seconds);
		strftime(t, sizeof(t), "%d.%m.%Y", timeinfo);
		strftime(t1, sizeof(t), "%X", timeinfo);
		if(t1=="00:00:00"){
			std::ofstream hist("history.txt", std::ios_base::app);
			hist<<"\n";
			hist<<t;
			hist.close();
		}
		Sleep(1000);
	}
}

void clientHandler(int index) {
	std::string onMsg="User "+users[index]+" connected";
	int sizeOfOnMsg=onMsg.size();
	for(int i = 0; i < Counter; i++) {
		if(i != index) {
			send(Connections[i], (char*)&sizeOfOnMsg, sizeof(int), NULL);
			send(Connections[i], onMsg.c_str(), sizeOfOnMsg, NULL);
		}
	}
	int msgSize;
	while(true) {
		if((recv(Connections[index], (char*)&msgSize, sizeof(int), NULL))>0){
			char *Cmsg = new char[msgSize+1];
			Cmsg[msgSize+1] = '\0';
			char t[st];
			time_t seconds=time(NULL);
			tm* timeinfo=localtime(&seconds);
			strftime(t, sizeof(t), "%X", timeinfo);
			recv(Connections[index], Cmsg, msgSize, NULL);
			std::string prov=Cmsg;
				if(prov=="/h"){
					std::string line, history;
					std::ifstream hist("history.txt");
					while (getline(hist, line)){
						history=history+line+"\n";
					}
    				hist.close();
    				int sizeH=history.size();
    				send(Connections[index], (char*)&sizeH, sizeof(int), NULL);
					send(Connections[index], history.c_str(), sizeH, NULL);
				}else if(prov=="/online"){
					std::string online;
					if(Counter==1){
						online="Unfortunately you are still the only user";
					}else{
						online="Users online:\n";
						for(int q=0; q<Counter; q++){
							if(q!=index){
								online+=users[q]+"\n";
							}
						}
					}
					int onlSize=online.size();
					send(Connections[index], (char*)&onlSize, sizeof(int), NULL);
					send(Connections[index], online.c_str(), onlSize, NULL);
				}else{
					char space[2]=" ";
					std::ostringstream gogo;
					gogo<<t;
					gogo<<space;
					gogo<<Cmsg;
					std::string msgWtime=gogo.str();
					std::ofstream hist("history.txt", std::ios_base::app);
					hist<<msgWtime;
					hist<<"\n";
					hist.close();
					msgSize+=9;
					for(int i = 0; i < Counter; i++) {
						if(i != index) {
							send(Connections[i], (char*)&msgSize, sizeof(int), NULL);
							send(Connections[i], msgWtime.c_str(), msgSize, NULL);
						}
					}
				}
			delete[] Cmsg;
		}else break;
	}
	std::string offMsg="User "+users[index]+" disconnected";
	int sizeOfOffMsg=offMsg.size();
	std::cout<<std::endl<<offMsg<<std::endl;
	closesocket(Connections[index]);
	for(int i = 0; i < Counter; i++) {
		if(i != index) {
			send(Connections[i], (char*)&sizeOfOffMsg, sizeof(int), NULL);
			send(Connections[i], offMsg.c_str(), sizeOfOffMsg, NULL);
		}
	}
	for(int i=index; i<Counter-1; i++){
		users[i]=users[i+1];
		Connections[i]=Connections[i+1];
	}
	Counter--;
}

int main(int argc, char* argv[]) {
	for(int i=0; i<maxCon; i++){
		users[i]="";
	}
	const int port=1111;
	char t[st];
	time_t seconds=time(NULL);
	tm* timeinfo=localtime(&seconds);
	strftime(t, sizeof(t), "%d.%m.%Y", timeinfo);
	std::ofstream hist("history.txt");
	hist<<t;
	hist<<"\n";
	hist.close();
	
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if(WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)timeHandler, NULL, NULL, NULL);
	SOCKADDR_IN adr;
	int adrSize = sizeof(adr);
	adr.sin_addr.s_addr = inet_addr("127.0.0.1");
	adr.sin_port = htons(port);
	adr.sin_family = AF_INET;

	SOCKET sockListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sockListen, (SOCKADDR*)&adr, sizeof(adr));
	listen(sockListen, SOMAXCONN);

	SOCKET newConnection;
	for(int i = 0; i < maxCon; i++) {
		newConnection = accept(sockListen, (SOCKADDR*)&adr, &adrSize);

		if(newConnection == 0) {
			std::cout << "Error"<<std::endl;
		} else {
			std::string HelloMsg = "Welcome to Chat! You can see the history if you send a message: '/h'. To see who's online, send: '/online'. \nTo finish chatting send: '/exit'.";
			int HelloMsgSize = HelloMsg.size();
			send(newConnection, (char*)&HelloMsgSize, sizeof(int), NULL);
			send(newConnection, HelloMsg.c_str(), HelloMsgSize, NULL);
			std::string online;
			if(Counter==0){
				online="Unfortunately you are still the only user";
			}else{
				online="Users online:\n";
				for(int q=0; q<=Counter; q++){
					online+=users[q]+"\n";
				}
			}
			int onlSize=online.size();
			send(newConnection, (char*)&onlSize, sizeof(int), NULL);
			send(newConnection, online.c_str(), onlSize, NULL);
			int logSize;
			recv(newConnection, (char*)&logSize, sizeof(int), NULL);
			char *login = new char[logSize+1];
			login[logSize+1] = '\0';
			recv(newConnection, login, logSize, NULL);
			std::string loginS=login;
			std::cout << "User "<<loginS<<" connected\n";
			users[i]=loginS;
			delete[] login;
			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, (LPVOID)(i), NULL, NULL);
		}
	}


	system("pause");
	return 0;
}
