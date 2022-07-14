#pragma once
#include "User.h"
class Message
{
private:
	std::string from_;
	std::string to_;
	std::string text_;

public:
	Message(const std::string& from, const std::string& to, const std::string& text) :
		from_(from), to_(to), text_(text) {}

	const std::string& getFrom() const { return from_; }
	const std::string& getTo() const { return to_; }
	const std::string& getText() const { return text_; }

	friend std::fstream& operator >>(std::fstream& is, Message& obj);
	friend std::ostream& operator <<(std::ostream& os, const Message& obj);
};