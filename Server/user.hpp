#pragma once
#include <string>

using namespace std;

class user
{
	string name_;
	size_t id_;
	string status_ = "None";
	string history_ = "Your history: \n";

	//Proposal
	size_t amount_ = 0;
	size_t price_ = 0;

	// Balance
	int dollars_ = 0;
	int rubbles_ = 0;
	
public:

	// Default constructor
	user() { name_ = "Default"; id_ = 0; }
	user(string name, size_t id) { name_ = name; id_ = id; }

	void setProposal(const string& type, size_t am, size_t price)
	{ 
		status_ = type;
		amount_ = am;
		price_ = price;
	}

	string showBalance()
	{
		return name_ + "    " + to_string(dollars_) + "$    " + to_string(rubbles_) + " RUB\n";
	}

	string name() { return name_; }
	size_t id() { return id_; }
	string status() { return status_; }
	size_t amount() { return amount_; }
	size_t price() { return price_; }
	string history() { return history_; }

	void setStatus(const string& st) { status_ = st; }
	void reduceAm(int n) { amount_ -= n; }
	void changeD(int n) { dollars_ += n; }
	void changeR(int n) { rubbles_ += n; }
	void addHistory(const string& s) { history_ += s; }
};

