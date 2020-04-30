#pragma once
#include <stack>
#include <unordered_map>
#include "Chunk.h"

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
	VM();
	bool interpret(Chunk* entryChunk);

	inline const std::vector<Value*>& getStack() const { return this->stack; }

private:
	Chunk* currentChunk;
	size_t ip;
	std::vector<Value*> stack;
public:
	std::unordered_map<std::string, Value*> globals;
	std::vector<Frame> frames;

private:
	void run(OpCode code);
	uint8_t advance();
	Value* typeCast(Value* val, ValueType to);
	inline void push_to_stack(Value* val);
};

