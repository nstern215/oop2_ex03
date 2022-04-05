#pragma once

#include <iostream>

using std::cout;
using std::cin;
using std::endl;

class MyException {

public: 

	virtual void show() {cout << "problom detected" << " ";}

};

class BadCommand : public MyException {
	
public:
	virtual void show() override {
		
		MyException::show();

		cout << "bad input try again." << endl;
		cout << "type help for instructions" << endl;
	}
};