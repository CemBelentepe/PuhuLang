//
// Created by cembelentepe on 08/09/22.
//

#pragma once

#include <string>
#include <vector>
class Scanner
{
public:
	explicit Scanner(const std::string& source);
	~Scanner();

	std::vector<Token> scan();

private:

private:
	std::string source;
};
