#pragma once

#include "Chunk.hpp"
#include "Value.hpp"
#include <vector>

struct Frame
{
	size_t ip;
	Chunk* chunk;
	size_t frameStart;

	Frame()
		:ip(0), chunk(nullptr), frameStart(0)
	{}

	Frame(size_t ip, Chunk* chunk, size_t frameStart)
		:ip(ip), chunk(chunk), frameStart(frameStart)
	{}
};

class VM
{
public:
	VM(std::vector<Data> globals);
	~VM();
	bool interpret(Chunk* entryChunk);

	inline const std::vector<Data>* getStack() const { return &this->stack; }

private:
	Chunk* currentChunk;
	size_t ip;
	std::vector<Data> stack;
public:
	std::vector<Data> globals;
	std::vector<Frame> frames;

private:
	inline uint8_t advance()
	{
		return this->currentChunk->code[this->ip++];
	}

	void typeCast(TypeTag from, TypeTag to);
};
