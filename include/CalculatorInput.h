#pragma once
#include <iosfwd>
#include <sstream>
#include <string>
#include <stack>

class CalculatorInput
{
public:
	CalculatorInput(std::istream& inputStream);
	
	void loadInput();
	void reset();

	void pushStreamForInput(std::istream* stream);
	
	std::string getLastInput() const;
	bool isUserMode() const;
private:
	
	std::stack<std::istream*> m_streams;
	std::stringstream m_lineInputStream;

	std::streambuf* m_originBuf;
	
	bool m_userOriginMode = true;
	
	std::string m_lastInput;
	std::istream& m_istr;
};
