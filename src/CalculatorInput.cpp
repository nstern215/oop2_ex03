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
	while (!std::getline(m_streams.empty() ? m_istr : *m_streams.top(), m_lastInput))
	{
		if (m_streams.empty())
			reset();
		else
		{
			m_streams.pop();
		}
	}

	m_lineInputStream.flush();
	m_lineInputStream << m_lastInput;

	m_istr.rdbuf(m_lineInputStream.rdbuf());
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

bool CalculatorInput::isUserMode() const
{
	return m_userOriginMode;
}
