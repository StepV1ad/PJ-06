#include "Message.h"

std::fstream& operator >>(std::fstream& is, Message& obj)
{
	is >> obj.from_;
	is >> obj.to_;
	is >> obj.text_;
	return is;
}
std::ostream& operator <<(std::ostream& os, const Message& obj)
{
	os << obj.from_;
	os << ' ';
	os << obj.to_;
	os << ' ';
	os << obj.text_;
	return os;
}