#pragma once
#include "List.h"
#include "Chunk.h"

#ifdef _DEBUG
#include <unordered_map>
#endif // _DEBUG


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
	~VM();
	bool interpret(Chunk* entryChunk);

	inline const List<uint8_t>* getStack() const { return &this->stack; }

private:
	Chunk* currentChunk;
	size_t ip;
	List<uint8_t> stack;
public:
	std::vector<uint8_t*> globals;
	std::vector<Frame> frames;
#ifdef _DEBUG
	std::unordered_map<std::string, Chunk***> globalFuncs;
#endif // _DEBUG


private:
	inline uint8_t advance()
	{
		return this->currentChunk->code[this->ip++];
	}

	void typeCast(ValueType from, ValueType to);
};

