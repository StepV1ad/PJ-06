#pragma once
#pragma comment (lib, "ws2_32.lib")
#include <vector>
#include <memory>
#include "Message.h"
#include <mysql.h>

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

#define MESSAGE_LENGTH 1024 // ћаксимальный размер буфера дл€ данных
#define PORT 7777 // Ѕудем использовать этот номер порта

class Chat
{
	std::vector<User> users_;
	std::vector<Message> commonChat_;
	std::vector<Message> messages_;
	std::shared_ptr<User> currentUser_ = nullptr;
	int idStorage_ = 0;
	bool chatWork_ = false;
	bool mainMenuWork_ = false;
	int socket_file_descriptor, connection;
	struct sockaddr_in serveraddress, client;
	char message[MESSAGE_LENGTH];

	bool checkLogin(const std::string& login);
	bool checkName(const std::string& name);
	bool checkPassword(const std::string& name, const std::string& password);
	void signUp();
	void login();

	void comChat();
	void addUserMessage();
	void showUserChat();
	void changeUser(); // измен€ет им€ или пароль пользовател€
	void showUsers(); // отображает список имЄн всех пользователей 
	void readUserFile();
	void wrightUserFile();
	void readMessageFile();
	void wrightMessageFile();
	void readCommonChatFile();
	void wrightCommonChatFile();

	void admin(); // зарегестрированный пользователь с отдельным меню // login admin, password 13579
	void printAllInf(); // отображает всю информацию пользователей
	void showPrivateChat(); // просмотр чатов пользователей

public:
	void socketTCP();
	void dataBase();
	void start();
	bool chatWork() const { return chatWork_; }
	bool mainMenuWork() const { return mainMenuWork_; }
	void showLoginMenu();
	void showUserMenu();
};