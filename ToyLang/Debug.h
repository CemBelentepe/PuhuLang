#pragma once
#include "Chunk.h"
#include "VM.h"

void dissableChunk(Chunk* chunk);
size_t dissambleInstruction(Chunk* chunk, size_t offset);
void printStack(VM& vm);