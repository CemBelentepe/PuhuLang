#include "Chunk.h"

Chunk::Chunk()
{}

size_t Chunk::addConstant(Value value)
{
	// TODO: check before add
	this->constants.push_back(value);
	return constants.size() - 1;
}

Value* Chunk::getConstant(size_t addr)
{
	return &this->constants[addr];
}
