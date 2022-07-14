#pragma once
#include <string>
#include <cstring>
#include <fstream>

class User
{
private:
	std::string login_;
	std::string password_;
	std::string name_;
	int id_;
public:
	User(const std::string& login, const std::string& password, const std::string& name, int id = 0) :
		login_(login), password_(password), name_(name), id_(id) {}

	const std::string& getUserLogin() const { return login_; }

	const std::string& getUserPassword() const { return password_; }
	void setUserPassword(const std::string& password) { password_ = password; }

	const std::string& getUserName() const { return name_; }
	void setUserName(const std::string& name) { name_ = name; }

	const int getID() { return id_; }
	void setID(int id) { id_ = id; }

	friend std::fstream& operator >>(std::fstream& is, User& obj);
	friend std::ostream& operator <<(std::ostream& os, const User& obj);
};