#include "Chunk.h"

Chunk::Chunk()
{}

Chunk::~Chunk()
{
	for (auto& val : constants)
	{
		delete val;
	}
}

size_t Chunk::addConstant(uint8_t* value, size_t valueSize)
{
	size_t p = 0;
	bool contains = false;
	for (int i = 0; i < constants.size() && !contains; i++)
	{
		bool same = true;
		for (int j = 0; j < valueSize && same; j++)
		{
			if (constants[i][j] != value[j])
				same = false;
		}
		if (same)
		{
			p = i;
			contains = true;
		}
	}
	if (contains)
		return p;
	else
	{
		this->constants.push_back(value);
		return this->constants.size() - 1;
	}
}

uint8_t* Chunk::getConstant(size_t addr)
{
	return this->constants[addr];
}
