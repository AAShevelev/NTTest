#pragma once
#include <string>

using namespace std;

class user
{
	public:
	string name_;
	size_t id_ = 0;
	string status_ = "";

	//Proposal
	size_t amount_ = 0;
	size_t price_ = 0;

	// Balance
	int dollars_ = 0;
	int rubbles_ = 0;

	// Default constructor
	user(string name = "") { name_ = name; };

	void setProposal(const string& type, size_t am, size_t price)
	{ 
		status_ = type;
		amount_ = am;
		price_ = price;
	};

	string showBalance()
	{
		return name_ + "    " + to_string(dollars_) + "$    " + to_string(rubbles_) + " RUB\n";
	}
};

