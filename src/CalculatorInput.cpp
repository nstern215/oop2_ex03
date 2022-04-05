#include "CalculatorInput.h"

#include <istream>
#include <sstream>

CalculatorInput::CalculatorInput(std::istream& inputStream) :
	m_istr(inputStream),
	m_originBuf(inputStream.rdbuf())
{}

std::string CalculatorInput::getLastInput() const
{
	return m_lastInput;
}

void CalculatorInput::loadInput()
{
	while (!std::getline(*getInputBuf(), m_lastInput))
		popStream();

	loadInputToStream();
}

void CalculatorInput::reset()
{
	for (std::istream* temp = nullptr; !m_streams.empty(); temp = m_streams.top())
	{
		delete temp;
		m_streams.pop();
	}

	m_istr.rdbuf(m_originBuf);
	m_userOriginMode = true;
}

void CalculatorInput::pushStreamForInput(std::istream* stream)
{
	m_streams.push(stream);
	m_userOriginMode = false;
}

bool CalculatorInput::isUserMode()
{
	while (!m_streams.empty() && isInputBufferEmpty())
		popStream();

	m_userOriginMode = m_streams.empty();
	
	return m_userOriginMode;
}

std::istream* CalculatorInput::getInputBuf() const
{
	if (m_streams.empty())
		return &m_istr;

	return m_streams.top();
}

void CalculatorInput::popStream()
{
	if (m_streams.empty())
		reset();
	else
		m_streams.pop();
}

void CalculatorInput::loadInputToStream()
{
	m_lineInputStream.flush();
	m_lineInputStream << m_lastInput;

	m_istr.rdbuf(m_lineInputStream.rdbuf());
}

bool CalculatorInput::isInputBufferEmpty()
{
	if (m_streams.empty())
		return true;

	char c;
	*m_streams.top() >> c;

	if (m_streams.top()->fail())
		return true;

	m_streams.top()->putback(c);
	return false;
}
