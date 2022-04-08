#include "SetCalculator.h"

#include "Union.h"
#include "Intersection.h"
#include "Difference.h"
#include "Product.h"
#include "Comp.h"
#include "Identity.h"

#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <exception>


namespace rng = std::ranges;

SetCalculator::SetCalculator(std::istream& istr, std::ostream& ostr)
	: m_actions(createActions()), m_operations(createOperations()), m_istr(istr), m_ostr(ostr), m_input(m_istr)
{
}

void SetCalculator::getListSize()
{
	m_ostr << "Please enter the size you would like for the list?" << std::endl;

	int i;
	m_input.loadInput();

	//todo: use m_istr instead
	std::cin >> i;

	//todo: set as const member (or define) in header file
	if ((i < 3) || (i > 100))
		throw std::out_of_range("The size is out of range. \n");

	if (i < m_operations.size())
		shortenList(i);

	m_listSize = i;

}


void SetCalculator::run()
{

	resize();

	do
	{
		if (m_input.isUserMode())
		{
			m_ostr << '\n';
			printOperations();
			m_ostr << "Enter command ('help' for the list of available commands): ";
		}
		runCalc();
	} while (m_running);
}

void SetCalculator::runCalc()
{
	m_input.loadInput();

	try
	{
		const auto action = readAction();
		runAction(action);
	}
	catch (std::invalid_argument& e)
	{
		m_ostr << e.what() << std::endl;

		if (!m_input.isUserMode())
		{
			m_ostr << m_input.getLastInput();

			auto nextStep = 0;

			m_ostr << "Please enter 1 to continue with file, or 0 to go back to userInput \n";

			//todo: load input in user mode
			m_istr >> nextStep;

			if (nextStep != 1)
				m_input.reset();

		}
	}
	catch (std::length_error& e) {

		//todo: replace with "\n"
		m_ostr << e.what() << std::endl;
		m_input.reset();
	}
	catch (std::ios_base::failure& e)
	{
		m_ostr << e.what() << std::endl;
	}
	catch (std::out_of_range& e)
	{
		m_ostr << e.what() << std::endl;

		//todo: use const member instead of 3
		if (m_operations.size() > 3)
			m_operations.pop_back();
	}

}

void SetCalculator::eval()
{
	if (auto index = readOperationIndex(); index)
	{
		const auto& operation = m_operations[*index];
		auto inputs = std::vector<Set>();
		for (auto i = 0; i < operation->inputCount(); ++i)
		{
			m_input.loadInput();
			inputs.push_back(Set(m_istr));


		}

		operation->print(m_ostr, inputs);
		m_ostr << " = " << operation->compute(inputs) << '\n';
	}
}

void SetCalculator::read()
{
	auto path = std::string();
	m_istr >> path;

	std::ifstream inputFile;

	inputFile.open(path);

	if (!inputFile.is_open())
		throw std::ios_base::failure("Cannot open and read file at: " + path);

	const auto fileStream = new std::stringstream();

	std::string line;
	while (!inputFile.eof())
	{
		std::getline(inputFile, line);
		*fileStream << line << std::endl;
	}

	inputFile.close();

	m_input.pushStreamForInput(fileStream);
}

void SetCalculator::resize()
{
	m_sizeIsValid = false;

	while (!m_sizeIsValid)
		try
	{
		getListSize();
		m_sizeIsValid = true;
	}
	catch (std::out_of_range& e)
	{
		m_ostr << e.what() << std::endl;
	}
}

void SetCalculator::shortenList(int& size)
{
	for (int i = m_operations.size(); m_operations.size() != size; i--)
	{
		m_operations.pop_back();
	}
}


void SetCalculator::del()
{

	if (auto i = readOperationIndex(); i)
	{
		if (m_operations.size() == 3)
			throw std::out_of_range("The list is currently at the least it can be.");

		m_operations.erase(m_operations.begin() + *i);
	}
}

void SetCalculator::help()
{
	m_ostr << "The available commands are:\n";
	for (const auto& action : m_actions)
	{
		m_ostr << "* " << action.command << action.description << '\n';
	}
	m_ostr << '\n';
}

void SetCalculator::exit()
{
	m_ostr << "Goodbye!\n";
	m_running = false;
}

void SetCalculator::printOperations() const
{
	m_ostr << "List of available set operations:\n";
	for (decltype(m_operations.size()) i = 0; i < m_operations.size(); ++i)
	{
		m_ostr << i << ".\t";
		auto gen = NameGenerator();
		m_operations[i]->print(m_ostr, gen);
		m_ostr << '\n';
	}
	m_ostr << '\n';
}

std::optional<int> SetCalculator::readOperationIndex() const
{
	auto i = 0;
	m_istr >> i;

	if (m_istr.fail())
		throw std::invalid_argument("This argument is not valid");

	if (i >= m_operations.size())
	{
		throw std::invalid_argument("This functions number is not valid");
	}

	return i;
}

SetCalculator::Action SetCalculator::readAction() const
{
	auto action = std::string();
	m_istr >> action;

	const auto i = std::ranges::find(m_actions, action, &ActionDetails::command);
	if (i != m_actions.end())
	{
		return i->action;
	}

	throw std::invalid_argument("This command does not exist try again \n");
}

void SetCalculator::runAction(Action action)
{
	//todo: check all functions for exception throwing\handling
	switch (action)
	{
	default:
		m_ostr << "Unknown enum entry used!\n";
		break;

	case Action::Eval:         eval();                     break;
	case Action::Union:        binaryFunc<Union>();        break;
	case Action::Intersection: binaryFunc<Intersection>(); break;
	case Action::Difference:   binaryFunc<Difference>();   break;
	case Action::Product:      binaryFunc<Product>();      break;
	case Action::Comp:         binaryFunc<Comp>();         break;
	case Action::Read:         read();                     break;
	case Action::Resize:       resize();                   break;
	case Action::Del:          del();                      break;
	case Action::Help:         help();                     break;
	case Action::Exit:         exit();                     break;
	}

	//todo: not belong here
	if (m_operations.size() > m_listSize)
		throw std::out_of_range("The list is currently full, there for cannot add the last operation.");
}

SetCalculator::ActionMap SetCalculator::createActions()
{
	return ActionMap
	{
		{
			"eval",
			"(uate) num ... - compute the result of function #num on the "
			"following set(s); each set is prefixed with the count of numbers to"
			" read",
			Action::Eval
		},
		{
			"uni",
			"(on) num1 num2 - Creates an operation that is the union of "
			"operation #num1 and operation #num2",
			Action::Union
		},
		{
			"inter",
			"(section) num1 num2 - Creates an operation that is the "
			"intersection of operation #num1 and operation #num2",
			Action::Intersection
		},
		{
			"diff",
			"(erence) num1 num2 - Creates an operation that is the "
			"difference of operation #num1 and operation #num2",
			Action::Difference
		},
		{
			"prod",
			"(uct) num1 num2 - Creates an operation that returns the product of"
			" the items from the results of operation #num1 and operation #num2",
			Action::Product
		},
		{
			"comp",
			"(osite) num1 num2 - creates an operation that is the composition "
			"of operation #num1 and operation #num2",
			Action::Comp
		},
		{
			"del",
			"(ete) num - delete operation #num from the operation list",
			Action::Del
		},
		{
			"read",
			" file - read and evaluate all including commands",
			Action::Read
		},
		{
			"resize",
			" list - shorten list or make list longer",
			Action::Resize
		},
		{
			"help",
			" - print this command list",
			Action::Help
		},
		{
			"exit",
			" - exit the program",
			Action::Exit
		}
	};
}

SetCalculator::OperationList SetCalculator::createOperations()
{
	return OperationList
	{
		std::make_shared<Union>(std::make_shared<Identity>(), std::make_shared<Identity>()),
		std::make_shared<Intersection>(std::make_shared<Identity>(), std::make_shared<Identity>()),
		std::make_shared<Difference>(std::make_shared<Identity>(), std::make_shared<Identity>())
	};
}

bool SetCalculator::listIsFull()
{
	if (m_operations.size() == (m_listSize - 1))
		return true;

	return false;
}
