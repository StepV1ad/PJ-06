#pragma once
#pragma comment (lib, "ws2_32.lib")
#include <vector>
#include <memory>
#include "Message.h"


#include <iostream>
#include <cstdio>
#include <stdio.h>
#if defined(__linux__)
#include <sys/utsname.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <VersionHelpers.h>
#include <WS2tcpip.h>
#include <winsock2.h>
#endif

#define MESSAGE_LENGTH 1024 // ������������ ������ ������ ��� ������
#define PORT 7777 // ����� ������������ ���� ����� �����

class Chat
{
	std::vector<User> users_;
	std::vector<Message> commonChat_;
	std::vector<Message> messages_;
	std::shared_ptr<User> currentUser_ = nullptr;
	int idStorage_ = 0;
	bool chatWork_ = false;
	bool mainMenuWork_ = false;
	struct sockaddr_in serveraddress, client;
	socklen_t length;
	int socket_file_descriptor, connection, bind_status, connection_status;
	char message[MESSAGE_LENGTH];

	bool checkLogin(const std::string& login);
	bool checkName(const std::string& name);
	bool checkPassword(const std::string& name, const std::string& password);
	void signUp();
	void login();

	void comChat();
	void addUserMessage();
	void showUserChat();
	void changeUser(); // �������� ��� ��� ������ ������������
	void showUsers(); // ���������� ������ ��� ���� ������������� 
	void readUserFile();
	void wrightUserFile();
	void readMessageFile();
	void wrightMessageFile();
	void readCommonChatFile();
	void wrightCommonChatFile();

	void admin(); // ������������������ ������������ � ��������� ���� // login admin, password 13579
	void printAllInf(); // ���������� ��� ���������� �������������
	void showPrivateChat(); // �������� ����� �������������

public:
	void dataBase();
	void socketTCP();
	void start();
	bool chatWork() const { return chatWork_; }
	bool mainMenuWork() const { return mainMenuWork_; }
	void showLoginMenu();
	void showUserMenu();
};