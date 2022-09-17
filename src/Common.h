//
// Created by cembelentepe on 12/09/22.
//

#pragma once
#include <memory>
#include <stdexcept>

class Type;
using TypePtr = std::shared_ptr<Type>;

class NotImplementedException : std::runtime_error
{
public:
	NotImplementedException()
		: std::runtime_error("Not Implemented.")
	{
	}
};