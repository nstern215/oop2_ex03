#pragma once

#include <iostream>

using std::cout;
using std::cin;
using std::endl;

class MyException {

public: 

	virtual void show() {cout << "PROBLEM DETECTED" << " ";}

};

class BadCommand : public MyException {
	
public:
	virtual void show() override {
		
		MyException::show();

		cout << "bad input try again." << endl << endl;

	}
};

class InvalidCommandNum : public MyException {

public:
	virtual void show() override {

		MyException::show();

		cout << "command number does not exist." << endl << endl;
		
	}
};