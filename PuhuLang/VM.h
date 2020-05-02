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
	~VM();
	bool interpret(Chunk* entryChunk);

	inline const std::vector<uint8_t>& getStack() const { return this->stack; }

private:
	Chunk* currentChunk;
	size_t ip;
	std::vector<uint8_t> stack;
public:
	std::unordered_map<std::string, uint8_t*> globals;
	std::vector<Frame> frames;
#ifdef _DEBUG
	std::unordered_map<std::string, Chunk***> globalFuncs;
#endif // _DEBUG


private:
	void run(OpCode code);
	uint8_t advance();

	void typeCast(ValueType from, ValueType to);

	template <class T>
	inline void push(T val)
	{
		size_t p = stack.size();
		stack.resize(p + sizeof(T));
		*(T*)(&stack[p]) = val;
	}

	inline void pushSized(uint8_t* val, size_t size)
	{
		if (size > 0)
		{
			this->stack.resize(this->stack.size() + size);
			memcpy(&stack[this->stack.size() - size], val, size * sizeof(uint8_t));
		}
		// for (int i = 0; i < size; i++)
		// {
		// 	this->stack.push_back(val[i]);
		// }
		// delete[] val;
	}

	template <class T>
	inline T pop()
	{
		size_t p = this->stack.size() - sizeof(T);
		T ret = *(T*)(&this->stack[p]);
		this->stack.resize(p);
		return ret;
	}

	inline uint8_t* popSized(size_t size)
	{
		// for (int i = 0; i < size; i++)
		// {
		// 	val[i] = stack[stack.size() -size + i];
		// }
		// stack.resize(stack.size() - size);
		if (size > 0)
		{
			uint8_t* val = &stack[stack.size() - size];
			return val;
		}
	}

	template <class T>
	inline T peek()
	{
		size_t p = this->stack.size() - sizeof(T);
		T ret = *(T*)(&this->stack[p]);
		return ret;
	}

	inline uint8_t* peekSized(size_t size)
	{
		return &stack[stack.size() - size];
	}
};

