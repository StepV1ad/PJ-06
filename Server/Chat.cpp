#include <iostream>
#include "Chat.h"
#include <mysql.h>

MYSQL mysql;
MYSQL_RES* res;
MYSQL_ROW row;
void Chat::dataBase()
{
	mysql_init(&mysql);
	if (&mysql == nullptr) 
		std::cout << "Error: can't create MySQL-descriptor" << std::endl;
	
	if (!mysql_real_connect(&mysql, "localhost", "root", "root", "testdb", NULL, NULL, 0)) 
		std::cout << "Error: can't connect to database " << mysql_error(&mysql) << std::endl;
	
	mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS user(id SERIAL PRIMARY KEY, name VARCHAR(80) not null, surname VARCHAR(80) not null), email VARCHAR(80) not null unique)");
	mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS pwd(id SERIAL PRIMARY KEY, user_id INT references use(id), password VARCHAR(80) not null)");
	mysql_query(&mysql, "CREATE TABLE IF NOT EXISTS message(mess_id SERIAL PRIMARY KEY, from_id int REFERENCES user(id), to_id int REFERENCES user(id), description TEXT, mess_date  DATE, status character varying(100) NOT NULL CHECK(status IN('done', 'in progress', 'error')))");
		
}

void Chat::socketTCP()
{
	socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_file_descriptor == -1) {
		std::cout << "Socket creation failed!" << std::endl;
		exit(1);
	}
	serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddress.sin_port = htons(PORT);
	serveraddress.sin_family = AF_INET;
	bind_status = bind(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
	if (bind_status == -1) {
		std::cout << "Socket binding failed!" << std::endl;
		exit(1);
	}

	connection_status = listen(socket_file_descriptor, 5);
	if (connection_status == -1) {
		std::cout << "Socket is unable to listen for new connections!" << std::endl;
		exit(1);
	}
	else { std::cout << "Server is listening for new connection: " << std::endl; }
	length = sizeof(client);
	connection = accept(socket_file_descriptor, (struct sockaddr*)&client, &length);
	if (connection == -1) {
		std::cout << "Server is unable to accept the data from client.!" << std::endl;
		exit(1);
	}
}

bool Chat::checkLogin(const std::string& login)
{
	for (auto& user : users_)
	{
		if (login == user.getUserLogin())
			return true;
	}
	return false;
}

bool Chat::checkName(const std::string& name)
{
	for (auto& user : users_)
	{
		if (name == user.getUserName())
			return true;
	}
	return false;
}

bool Chat::checkPassword(const std::string& login, const std::string& password)
{
	for (auto& user : users_)
	{
		if (login == user.getUserLogin())
			return (password == user.getUserPassword()) ? true : false;
	}
	return false;
}

void Chat::signUp()
{
	std::string login, password, name;
	bool flag = false;

	do
	{
		std::cout << "Enter login: ";
		std::cin >> login;
		std::cout << "Enter password: ";
		std::cin >> password;
		std::cout << "Enter name: ";
		std::cin >> name;
		if (checkLogin(login))
		{
			std::cout << "ERROR: user login is busy! Try again" << "\n" << "\n";
		}
		else if (checkName(name))
		{
			std::cout << "ERROR: user name is busy! Try again" << "\n" << "\n";
		}
		else
		{
			flag = true;
			std::cout << std::endl;
		}
	} while (flag == false);

	User user = User(login, password, name);
	user.setID(++idStorage_);
	users_.emplace_back(user);
	currentUser_ = std::make_shared<User>(user);
}

void Chat::login()
{
	std::string login, password;
	char operation;
	bool flag = false;
	do
	{
		std::cout << "Enter login: ";
		std::cin >> login;
		std::cout << "Enter password: ";
		std::cin >> password;

		if ((!checkLogin(login)) && (!checkPassword(login, password)))
		{
			std::cout << "Login or password failed! Try again(any key) or exit(0): " << "\n";
			std::cin >> operation;
			if (operation == '0')
				break;
		}
		else
		{
			for (auto& user : users_)
			{
				if (login == user.getUserLogin())
				{
					currentUser_ = std::make_shared<User>(user);
				}
			}
			flag = true;
			std::cout << "\n";
		}
	} while (flag == false);
}

void Chat::comChat()
{
	char message[MESSAGE_LENGTH];
	std::string text;
	std::cout << "--- Chat ---" << std::endl;
	while (1) {

		bzero(message, MESSAGE_LENGTH);
		recv(connection, message, sizeof(message), 0);
		if (strncmp("end", message, 3) == 0) {
			std::cout << "Client Exited." << std::endl;
			std::cout << "Server is Exiting..!" << std::endl;
			std::cout << "---------" << std::endl << std::endl;
			break;
		}
		std::cout << "Data received from client: " << message << std::endl;
		bzero(message, MESSAGE_LENGTH);
		std::cout << "Enter the message you want to send to the client: " << std::endl;

		//std::cin.ignore();
		getline(std::cin, text);

		strcpy(message, text.c_str());

		ssize_t bytes = send(connection, message, sizeof(message), 0);
		if (bytes >= 0)
			std::cout << "Data successfully sent to the client!" << std::endl;

	}
}

void Chat::addUserMessage()
{
	std::string to, text;

	std::cout << "To: ";
	std::cin >> to;
	std::cout << "Text: ";
	std::cin.ignore();
	getline(std::cin, text);

	if (!checkName(to))
	{
		std::cout << "Error send message: can't find " << to << std::endl;
		return;
	}
	else
		messages_.emplace_back(Message{ currentUser_->getUserName(), to, text });
}

void Chat::showUserChat()
{
	std::cout << "--- Chat ---" << std::endl;

	for (auto& mess : messages_)
	{
		if (currentUser_->getUserName() == mess.getFrom())
		{
			std::cout << "Message from " << currentUser_->getUserName() << " to " << mess.getTo() << std::endl;
			std::cout << "text: " << mess.getText() << std::endl;
		}
		if (currentUser_->getUserName() == mess.getTo())
		{
			std::cout << "Message from " << mess.getFrom() << " to " << currentUser_->getUserName() << std::endl;
			std::cout << "text: " << mess.getText() << std::endl;
		}
	}
	std::cout << "---------" << std::endl;
}

void Chat::changeUser()
{
	std::string newU;
	char operation;
	std::cout << "Change (1)name or (2)password: ";
	std::cin >> operation;
	switch (operation)
	{
	case '1':
		std::cout << "Enter new name: ";
		std::cin >> newU;
		if (checkName(newU))
			std::cout << "ERROR: user name is busy! Try again" << "\n";
		else
			currentUser_->setUserName(newU);
		break;
	case '2':
		std::cout << "Enter new password: ";
		std::cin >> newU;
		currentUser_->setUserPassword(newU);
		break;
	default:
		std::cout << "unknown choice" << std::endl;
		break;
	}
	std::cout << std::endl;
	users_.erase(users_.begin() + currentUser_->getID());
	users_.emplace(users_.begin() + currentUser_->getID(), User(currentUser_->getUserLogin(), currentUser_->getUserPassword(), currentUser_->getUserName(), currentUser_->getID()));
}

void Chat::showUsers()
{
	for (size_t inf = 1; inf < users_.size(); ++inf) // не используется foreach для сокрытия admin
	{
		std::cout << "User #" << inf << ": " << users_[inf].getUserName() << "\n";
	}
	std::cout << std::endl;
}

void Chat::readUserFile()
{
	std::string login, password, name;
	int idStorage;
	std::ifstream user_file = std::ifstream("users.txt");
	if (user_file.is_open())
	{
		while (user_file >> idStorage >> login >> password >> name)
			users_.emplace_back(User(login, password, name));
		idStorage_ = idStorage;
	}
}

void Chat::wrightUserFile()
{
	int idStorage = 0;
	std::ofstream user_file = std::ofstream("users.txt");
	if (user_file.is_open())
		for (auto& user : users_)
			user_file << idStorage++ << " " << user.getUserLogin() << " " << user.getUserPassword() << " " << user.getUserName() << std::endl;
}

void Chat::readMessageFile()
{
	std::string from, to, text;
	std::ifstream messages_file = std::ifstream("messages.txt");
	if (messages_file.is_open())
		while ((messages_file >> from >> to) && getline(messages_file, text))
			messages_.emplace_back(Message{ from, to, text });
}

void Chat::wrightMessageFile()
{
	std::ofstream messages_file = std::ofstream("messages.txt");
	if (messages_file.is_open())
		for (auto& messages : messages_)
			messages_file << messages.getFrom() << " " << messages.getTo() << " " << messages.getText() << std::endl;
}

void Chat::readCommonChatFile()
{
	std::string from, to, text;
	std::ifstream commonChat_file = std::ifstream("commonChat.txt");
	if (commonChat_file.is_open())
		while ((commonChat_file >> from >> to) && getline(commonChat_file, text))
			commonChat_.emplace_back(Message{ from, to, text });
}

void Chat::wrightCommonChatFile()
{
	std::ofstream commonChat_file = std::ofstream("commonChat.txt");
	if (commonChat_file.is_open())
		for (auto& commonChat : commonChat_)
			commonChat_file << commonChat.getFrom() << " " << commonChat.getTo() << " " << commonChat.getText() << std::endl;
}

void Chat::admin() // доп пользователь, регистрируемый при старте работы чата
{
	for (auto& user : users_)
	{
		if ("admin" == user.getUserLogin())
			return;
	}
	User user = User("admin", "13579", "admin");
	user.setID(0);
	users_.emplace_back(user);
}

void Chat::printAllInf()
{
	for (size_t inf = 1; inf < users_.size(); ++inf)
	{
		std::cout << "User ID: " << users_[inf].getID()
			<< " Name: " << users_[inf].getUserName()
			<< " Login: " << users_[inf].getUserLogin()
			<< " Password: " << users_[inf].getUserPassword() << std::endl;
	}
	std::cout << std::endl;
}

void Chat::showPrivateChat()
{
	std::string name;

	std::cout << "Enter verifiable name: ";
	std::cin >> name;
	if (!checkName(name))
		std::cout << "ERROR: user name is missing! Try again" << "\n";
	else
	{
		std::cout << "--- Check Chat ---" << std::endl;

		for (auto& mess : messages_)
		{
			if (name == mess.getFrom())
			{
				std::cout << "Message from " << name << " to " << mess.getTo() << std::endl;
				std::cout << "text: " << mess.getText() << std::endl;
			}
			if (name == mess.getTo())
			{
				std::cout << "Message from " << mess.getFrom() << " to " << name << std::endl;
				std::cout << "text: " << mess.getText() << std::endl;
			}
		}
		std::cout << "---------" << std::endl;
	}
}

void Chat::start()
{
	socketTCP();
	chatWork_ = true;
	readUserFile();
	readMessageFile();
	readCommonChatFile();
	admin();
}

void Chat::showLoginMenu()
{
	currentUser_ = nullptr;
	char operation;

	do
	{
		std::cout << "Login menu \n"
			" (1)Login\n"
			" (2)Signup\n"
			" (0)Shutdown\n"
			"select operation: ";
		std::cin >> operation;
		std::cout << std::endl;
		switch (operation)
		{
		case '1':
			login();
			mainMenuWork_ = true;
			break;
		case '2':
			signUp();
			mainMenuWork_ = true;
			break;
		case '0':
			wrightUserFile();
			wrightMessageFile();
			wrightCommonChatFile();
			close(socket_file_descriptor);
			chatWork_ = false;
			break;
		default:
			std::cout << "1 or 2..." << std::endl;
			break;
		}
	} while (chatWork_ && !mainMenuWork_);
}

void Chat::showUserMenu()
{
	char operation;
	std::cout << "Hello, " << currentUser_->getUserName() << std::endl;
	if (currentUser_->getUserName() != "admin")
	{
		while (mainMenuWork_)
		{
			std::cout << "User menu \n"
				" (1)Show common chat \n"
				" (2)Show user chat \n"
				" (3)Add user message \n"
				" (4)Change user name or password \n"
				" (5)Show all users \n"
				" (0)Logout \n"
				"select operation: ";
			std::cin >> operation;
			std::cout << std::endl;
			switch (operation)
			{
			case '1':
				comChat();
				break;
			case '2':
				showUserChat();
				break;
			case '3':
				addUserMessage();
				break;
			case '4':
				changeUser();
				break;
			case '5':
				showUsers();
				break;
			case '0':
				mainMenuWork_ = false;
				currentUser_ = nullptr;
				break;
			default:
				std::cout << "unknown choice..." << std::endl;
				break;
			}
		}
	}
	else
	{
		while (mainMenuWork_)
		{
			std::cout << "User menu \n"
				" (1)Show common chat \n"
				" (2)Show user chat \n"
				" (3)Add user message \n"
				" (4)Print all information about users \n"
				" (5)Show private chat \n"
				" (0)Logout \n"
				"select operation: ";
			std::cin >> operation;
			std::cout << std::endl;
			switch (operation)
			{
			case '1':
				comChat();
				break;
			case '2':
				showUserChat();
				break;
			case '3':
				addUserMessage();
				break;
			case '4':
				printAllInf();
				break;
			case '5':
				showPrivateChat();
				break;
			case '0':
				mainMenuWork_ = false;
				currentUser_ = nullptr;
				break;
			default:
				std::cout << "unknown choice..." << std::endl;
				break;
			}
		}
	}
}