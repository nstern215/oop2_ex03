#pragma once

#include <vector>
#include <memory>
#include <string>
#include <iosfwd>
#include <optional>

#include "CalculatorInput.h"

class Operation;

class SetCalculator
{
public:
    SetCalculator(std::istream& istr, std::ostream& ostr);
    void run();


private:
    void eval();
    void read();
    void resize();
    void del();
    void help();
    void exit();

    template <typename FuncType>
    void binaryFunc()
    {
    	//todo: check if there enough space to add a new operation
        if (auto f0 = readOperationIndex(), f1 = readOperationIndex(); f0 && f1)
        {
            m_operations.push_back(std::make_shared<FuncType>(m_operations[*f0], m_operations[*f1]));
        }
    }

    void printOperations() const;

    enum class Action
    {
        Invalid,
        Eval,
        Union,
        Intersection,
        Difference,
        Product,
        Comp,
        Del,
    	Read,
        Resize,
        Help,
        Exit,
    };

    struct ActionDetails
    {
        std::string command;
        std::string description;
        Action action;
    };

    using ActionMap = std::vector<ActionDetails>;
    using OperationList = std::vector<std::shared_ptr<Operation>>;

    const ActionMap m_actions;
    OperationList m_operations;
    bool m_running = true;
    std::istream& m_istr;
    std::ostream& m_ostr;

    std::optional<int> readOperationIndex() const;
    Action readAction() const;
    void runAction(Action action);
    void runCalc();

    void getListSize();
    void shortenList(int& size);
	bool listIsFull();


    CalculatorInput m_input;
	
    static ActionMap createActions();
    static OperationList createOperations();

    bool m_sizeIsValid;
    int m_listSize;
};
