//
// Created by cembelentepe on 12/09/22.
//

#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <sstream>
#include "Token.h"

template<typename T>
class Environment
{
public:
	explicit Environment(std::unique_ptr<Environment> parent)
		: parent(std::move(parent)), symbolTable()
	{
	}

	T getVariable(const Token& nameToken) const
	{
		auto it = symbolTable.find(nameToken.getLexeme());
		if (it != symbolTable.end())
			return it->second;
		else if (parent)
			return parent->getVariable(nameToken);
		else
		{
			std::stringstream ssErr;
			ssErr << "[ERROR " << nameToken.line << ":" << nameToken.col << "] Use of undeclared identifier `"
				  << nameToken.lexeme << "`.";
			throw std::runtime_error(ssErr.str());
		}
	}

	void addVariable(const Token& nameToken, const T& init)
	{
		std::string name = nameToken.getLexeme();
		auto it = symbolTable.find(name);
		if (it != symbolTable.end())
		{
			std::stringstream ssErr;
			ssErr << "[ERROR " << nameToken.line << ":" << nameToken.col << "] Redefinition of `" << nameToken.lexeme
				  << "`.";
			throw std::runtime_error(ssErr.str());
		}

		symbolTable[name] = init;
	}

	void setVariable(const Token& nameToken, const T& init)
	{
		auto it = symbolTable.find(nameToken.getLexeme());
		if (it != symbolTable.end())
			it->second = init;
		else if (parent)
			parent->setVariable(nameToken, init);
		else
		{
			std::stringstream ssErr;
			ssErr << "[ERROR " << nameToken.line << ":" << nameToken.col << "] Use of undeclared identifier `"
				  << nameToken.lexeme << "`.";
			throw std::runtime_error(ssErr.str());
		}
	}

	std::unique_ptr<Environment> moveParent()
	{
		return std::move(parent);
	}

private:
	std::unique_ptr<Environment> parent;
	std::unordered_map<std::string, T> symbolTable;
};
