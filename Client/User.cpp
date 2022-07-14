#include "User.h"

std::fstream& operator >>(std::fstream& is, User& obj)
{
	is >> obj.name_;
	is >> obj.login_;
	is >> obj.password_;
	return is;
}
std::ostream& operator <<(std::ostream& os, const User& obj)
{
	os << obj.name_;
	os << ' ';
	os << obj.login_;
	os << ' ';
	os << obj.password_;
	return os;
}