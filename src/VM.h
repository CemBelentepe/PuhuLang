#pragma once

#include "ArrayList.hpp"
#include "Chunk.hpp"
#include "Value.hpp"

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
	VM(ArrayList<uint8_t> globals);
	~VM();
	bool interpret(Chunk* entryChunk);

	inline const ArrayList<uint8_t>* getStack() const { return &this->stack; }

private:
	Chunk* currentChunk;
	size_t ip;
	ArrayList<uint8_t> stack;
public:
	ArrayList<uint8_t> globals;
	std::vector<Frame> frames;

private:
	inline uint8_t advance()
	{
		return this->currentChunk->code[this->ip++];
	}

	void typeCast(TypeTag from, TypeTag to);
};
