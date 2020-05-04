#include "Compiler.h"
#include "Natives.h"
#include <sstream>

Compiler::Compiler(std::vector<Token>& tokens, VM& vm)
	:tokens(tokens), currentToken(0), vm(vm), compilingChunk(nullptr), scopeDepth(0), frame(0)
{}

Chunk* Compiler::compile()
{
	compilingChunk = new Chunk();
	addNatives();
	firstPass();
	this->currentToken = 0;

	while (!match(TokenType::EOF_TOKEN))
	{
		// compileExpression();
		decleration();
	}

	addCode(OpCode::GET_GLOBAL, sizeof(char*));
	addCode(getGlobal("main"));
	addCode(OpCode::CALL, 0);

	vm.globals.resize(globals.size());
	for (auto& var : globals)
	{
		vm.globals[var.second.second] = var.second.first->cloneData();
#ifdef _DEBUG
		if (var.second.first->type.type == ValueType::FUNCTION)
			vm.globalFuncs[var.first] = (Chunk***)var.second.first->cloneData();
#endif // _DEBUG
	}

	return compilingChunk;
}

void Compiler::addNatives()
{
	addGlobal("print", new NativeFunc(native_print, ValueType::VOID, -1));
	addGlobal("input", new NativeFunc(native_input, ValueType::STRING, 0));
	addGlobal("clock", new NativeFunc(native_clock, ValueType::DOUBLE, 0));
	
	addGlobal("inputInt", new NativeFunc(native_inputInt, ValueType::INTEGER, 0));

	// addGlobal("println", new NativeFunc(native_println, ValueType::VOID, 1));
	// addGlobal("printlnDouble", new NativeFunc(native_printlnDouble, ValueType::VOID, 1));
	// addGlobal("printlnInt", new NativeFunc(native_printlnInt, ValueType::VOID, 1));
}

void Compiler::firstPass()
{
	while (!match(TokenType::EOF_TOKEN))
	{
		parseDeclarations();
	}
}

void Compiler::parseDeclarations()
{
	if (peek().type == TokenType::CLASS)
		parseClassDeclaration();
	else
		parseFunctionDeclaration();
}

void Compiler::parseFunctionDeclaration()
{
	DataType type = parseTypeName();
	Token name = advance();
	if (peek().type == TokenType::OPEN_PAREN)
	{
		// TODO: Create a functions here
		std::stringstream signiture;
		signiture << name.getString();
		advance();
		size_t argSize = 0;
		if (peek().type != TokenType::CLOSE_PAREN)
		{
			argSize += parseTypeName().getSize();
			advance();
		}
		while (!match(TokenType::CLOSE_PAREN))
		{
			// signiture << "::" << parseTypeName();
			consume(TokenType::COMMA, "Expect ',' between parameters.");
			argSize += parseTypeName().getSize();
			advance();
		}
		addGlobal(signiture.str(), new FuncValue(nullptr, type, argSize));
		consume(TokenType::OPEN_BRACE, "Expect '{' at function start."); // Opening {
		size_t blocks = 1;
		while (blocks > 0)
		{
			TokenType type = advance().type;
			if (type == TokenType::OPEN_BRACE)
				blocks++;
			else if (type == TokenType::CLOSE_BRACE)
				blocks--;
		}
	}
	else
	{
		parseGlobalDeclaration(type, name);
	}
}

void Compiler::parseGlobalDeclaration(DataType type, Token name)
{
	addGlobal(name.getString(), new GlobalVariable(type));
	while (peek().type != TokenType::SEMI_COLON)
		advance();
	advance();
}

DataType Compiler::parseTypeName()
{
	DataType type;
	ValueType val = getDataType(peek());
	int i = 0;
	do
	{
		if (i > 0)
			type.intrinsicType = std::make_shared<DataType>(type);
		type.type = val;
		if (type.isPrimitive() && type.intrinsicType != nullptr && type.intrinsicType->isPrimitive())
		{
			error("Invalid type name.");
			break;
		}

		advance();
		i++;
	} while ((val = getDataType(peek())) != ValueType::ERROR);

	return type;
}

bool Compiler::isTypeName(Token& token)
{
	switch (token.type)
	{
	case TokenType::INT:
	case TokenType::FLOAT:
	case TokenType::DOUBLE:
	case TokenType::CHAR:
	case TokenType::STRING:
	case TokenType::BOOL:
	case TokenType::VOID:
		return true;
	default:
		return false;
	}
}

ValueType Compiler::getDataType(Token& token)
{
	switch (token.type)
	{
	case TokenType::INT:
		return ValueType::INTEGER;
	case TokenType::FLOAT:
		return ValueType::FLOAT;
	case TokenType::DOUBLE:
		return ValueType::DOUBLE;
	case TokenType::CHAR:
		return ValueType::CHAR;
	case TokenType::STRING:
		return ValueType::STRING;
	case TokenType::BOOL:
		return ValueType::BOOL;
	case TokenType::VOID:
		return ValueType::VOID;
	case TokenType::STAR:
		return ValueType::POINTER;
	case TokenType::OPEN_BRACKET:
		if (peekNext().type == TokenType::CLOSE_BRACKET)
		{
			advance();
			return ValueType::ARRAY;
		}
		else
			return ValueType::ERROR;
	default:
		return ValueType::ERROR;
	}
}

void Compiler::consume(TokenType type, const char* message)
{
	if (tokens[currentToken].type == type)
	{
		advance();
	}
	else
	{
		errorAtToken(message);
	}

}

void Compiler::consumeNext(TokenType type, const char* message)
{
	if (tokens[currentToken + 1].type == type)
	{
		advance();
	}
	else
	{
		errorAtToken(message);
	}

}

void Compiler::statement()
{
	TokenType token = peek().type;
	if (token == TokenType::OPEN_BRACE)
	{
		beginScope();
		block();
		endScope();
	}
	else if (token == TokenType::IF)
	{
		ifStatement();
	}
	else if (token == TokenType::WHILE)
	{
		whileStatement();
	}
	else if (token == TokenType::FOR)
	{
		forStatement();
	}
	else if (token == TokenType::RETURN)
	{
		advance();
		size_t size = compileExpression().getSize();
		consume(TokenType::SEMI_COLON, "Expect ';' after an expression statement.");
		addCode(OpCode::RETURN, size);
	}
	else
	{
		compileExpression();
		consume(TokenType::SEMI_COLON, "Expect ';' after an expression statement.");
	}
}

void Compiler::decleration()
{
	TokenType token = peek().type;
	if (token == TokenType::CLASS)
	{
		// TODO parse a class
	}
	else if (token == TokenType::NAMESPACE)
	{
		// TODO add namespaces
	}
	else
	{
		DataType typeName = parseTypeName();
		if (peekNext().type == TokenType::OPEN_PAREN)
		{
			functionDecleration(typeName);
		}
		else
		{
			variableDecleration(typeName);
		}
	}

}

void Compiler::variableDecleration(DataType type)
{
	std::string name = advance().getString();

	if (this->scopeDepth == 0)
		((GlobalVariable*)globals[name].first)->initialized = true;
	else
	{
		for (int i = this->locals.size() - 1; i >= this->frame; i--)
		{
			LocalVariable& local = locals[i];
			if (local.depth < scopeDepth)
				break;
			if (local.name == name)
				error("Variable with this name is already declared on this scope.");
		}
		addLocal(type, name, scopeDepth);
	}

	if (match(TokenType::EQUAL))
	{
		DataType eqType = compileExpression();
		if (eqType != type)
			emitCast(eqType, type);
		if (this->scopeDepth == 0)
		{
			addCode(OpCode::SET_GLOBAL, eqType.getSize());
			addCode(getGlobal(name));
		}
		else
		{
			addCode(OpCode::SET_LOCAL);
			addCode(locals[locals.size() - 1].startPos);
			addCode(locals[locals.size() - 1].type.getSize());
		}
	}

	consume(TokenType::SEMI_COLON, "Expect ';' after variable decleration.");
}

void Compiler::functionDecleration(DataType type)
{
	Chunk* enclosing = compilingChunk;
	std::string funcName = advance().getString();
	compilingChunk = new Chunk();
	((FuncValue*)globals[funcName].first)->chunk = compilingChunk;

	int frameStart = this->locals.size();
	this->frame = frameStart;

	advance(); // OPEN_PAREN

	beginScope();
	addLocal(ValueType::FUNCTION, "", this->scopeDepth);
	if (peek().type != TokenType::CLOSE_PAREN)
	{
		DataType type = parseTypeName();
		std::string name = advance().getString();
		addLocal(type, name, scopeDepth);
	}
	while (!match(TokenType::CLOSE_PAREN))
	{
		advance(); // consume ','
		DataType type = parseTypeName();
		std::string name = advance().getString();
		addLocal(type, name, scopeDepth);
	}
	block();
	// endScope();
	this->scopeDepth--;

	addCode(OpCode::RETURN, 0);

	this->locals.resize(frameStart);
	this->frame = frameStart;
	compilingChunk = enclosing;
}

void Compiler::block()
{
	advance();
	while (peek().type != TokenType::CLOSE_BRACE && peek().type != TokenType::EOF_TOKEN)
	{
		Token next = peek();
		if (isTypeName(next))
			variableDecleration(parseTypeName());
		else
			statement();
	}
	consume(TokenType::CLOSE_BRACE, "Expect '}' after a block.");
}

void Compiler::ifStatement()
{
	advance(); // if token
	consume(TokenType::OPEN_PAREN, "Expect '(' after 'if'.");
	DataType expr = compileExpression();
	if (expr.type != ValueType::BOOL)
		typeError("If statement expects boolean in its expression.");
	consume(TokenType::CLOSE_PAREN, "Expect ')' after if expression.");

	addCode(OpCode::JUMP_NT_POP);
	addCode(0);
	size_t jmp_pos_if = compilingChunk->code.size();
	statement();

	if (match(TokenType::ELSE))
	{
		addCode(OpCode::JUMP);
		addCode(0);
		size_t jmp_pos_else = compilingChunk->code.size();
		statement();
		compilingChunk->code[jmp_pos_else - 1] = compilingChunk->code.size() - jmp_pos_else;
		compilingChunk->code[jmp_pos_if - 1] = jmp_pos_else - jmp_pos_if;
	}
	else
	{
		compilingChunk->code[jmp_pos_if - 1] = compilingChunk->code.size() - jmp_pos_if;
	}
}

void Compiler::whileStatement()
{
	advance(); // Consume 'while' token
	consume(TokenType::OPEN_PAREN, "Expect '(' after 'while'.");
	size_t startPos = this->compilingChunk->code.size() - 1;
	DataType expr = compileExpression();
	if (expr.type != ValueType::BOOL)
		typeError("If statement expects boolean in its expression.");
	consume(TokenType::CLOSE_PAREN, "Expect ')' after while expression.");
	addCode(OpCode::JUMP_NT_POP);
	addCode(0);
	size_t loopPos = this->compilingChunk->code.size() - 1;
	statement();
	addCode(OpCode::LOOP);
	addCode(this->compilingChunk->code.size() - startPos);

	this->compilingChunk->code[loopPos] = this->compilingChunk->code.size() - loopPos - 1;
}

void Compiler::forStatement()
{
	advance(); // Consume 'for' token
	consume(TokenType::OPEN_PAREN, "Expect '(' after 'for'.");
	beginScope();

	size_t declerationSize = 0;

	if (!match(TokenType::SEMI_COLON))
	{
		if (isTypeName(peek()))
		{
			DataType typeName = parseTypeName();
			declerationSize = typeName.getSize();
			variableDecleration(typeName);
		}
		else
		{
			compileExpression();
			consume(TokenType::SEMI_COLON, "Expect ';' for 'for' decleration.");
		}
	}

	size_t cond = this->compilingChunk->code.size() - 1;
	if (!match(TokenType::SEMI_COLON))
	{
		DataType type = compileExpression();
		if (type != ValueType::BOOL)
			typeError("Expected a boolean expression for the second expression in for.");

		consume(TokenType::SEMI_COLON, "Expect ';' for 'for' condition.");
	}
	else
	{
		Value c = Value(true);
		addCode(OpCode::CONSTANT, c.type.getSize());
		addCode(compilingChunk->addConstant(c.cloneData(), c.type.getSize()));
	}
	addCode(OpCode::JUMP_NT_POP);
	addCode(0);
	size_t jnt_out = this->compilingChunk->code.size() - 1;
	addCode(OpCode::JUMP);
	addCode(0);
	size_t jmp_start = this->compilingChunk->code.size() - 1;

	size_t inc = this->compilingChunk->code.size() - 1;
	if (!match(TokenType::CLOSE_PAREN))
	{
		compileExpression();
		consume(TokenType::CLOSE_PAREN, "Expect ')' at the end of 'for'.");
	}
	addCode(OpCode::LOOP);
	addCode(0);
	size_t loop_cond = this->compilingChunk->code.size() - 1;

	size_t body = this->compilingChunk->code.size() - 1;
	block();
	endScope();
	this->compilingChunk->code.back() -= declerationSize;
	addCode(OpCode::LOOP);
	addCode(0);
	size_t loop_inc = this->compilingChunk->code.size() - 1;
	size_t out = this->compilingChunk->code.size() - 1;


	this->compilingChunk->code[jnt_out] = out - jnt_out;
	this->compilingChunk->code[jmp_start] = body - jmp_start;
	this->compilingChunk->code[loop_cond] = loop_cond - cond;
	this->compilingChunk->code[loop_inc] = loop_inc - inc;
}

uint8_t Compiler::identifierConstant(std::string& name)
{
	StrValue c(name);
	return this->compilingChunk->addConstant(c.cloneData(), c.type.getSize());
}

void Compiler::beginScope()
{
	this->scopeDepth++;
}

void Compiler::endScope()
{
	size_t start = this->locals.back().startPos + this->locals.back().type.getSize();
	for (int i = this->locals.size() - 1; i >= 0; i--)
	{
		if (this->locals[i].depth == this->scopeDepth)
		{
			this->locals.pop_back();
		}
		else break;
	}
	addCode(OpCode::POPN);
	addCode(start - (this->locals.back().startPos + this->locals.back().type.getSize()));

	this->scopeDepth--;
}

void Compiler::addLocal(DataType type, std::string name, int depth)
{
	size_t pos = 0;
	if (this->locals.size() > 0)
	{
		pos = this->locals.back().startPos + this->locals.back().type.getSize();
	}

	this->locals.push_back(LocalVariable(type, name, depth, pos));
}

size_t Compiler::addGlobal(std::string name, Value* value)
{
	size_t pos = globals.size();
	globals[name] = std::make_pair(value, pos);
	return pos;
}

size_t Compiler::getGlobal(std::string name)
{
	return globals[name].second;
}

DataType Compiler::compileExpression()
{
	if (peek().type == TokenType::IDENTIFIER && peekNext().type == TokenType::EQUAL)
	{
		Token token = advance();

		consume(TokenType::EQUAL, "Expect '=' for assignment.");
		DataType expType = compileExpression();

		DataType type = ValueType::ERROR;
		int slot = -1;
		for (int i = this->locals.size() - 1; i >= frame; i--)
		{
			if (this->locals[i].name == token.getString())
			{
				slot = i;
				break;
			}
		}

		// Not Local
		if (slot == -1)
		{
			auto it = globals.find(token.getString());
			// Global
			if (it != globals.end())
			{
				type = globals[token.getString()].first->type;
				if (expType != type)
					emitCast(expType, type);
				addCode(OpCode::SET_GLOBAL_POP, type.getSize());
				addCode(getGlobal(token.getString()));
				addCode(type.getSize());
			}
			// DNE
			else
			{
				return typeError("Identifier is not defined.");
			}
		}
		// Local
		else
		{
			type = this->locals[slot].type;
			if (expType != type)
				emitCast(expType, type);
			addCode(OpCode::SET_LOCAL_POP, locals[slot].startPos, type.getSize());
		}
		return type;
	}
	else
		return logic_or();
}

DataType Compiler::logic_or()
{
	DataType a = logic_and();
	if (a == ValueType::ERROR) return a;
	while (match(TokenType::OR))
	{
		addCode(OpCode::JUMP_NT);
		addCode(2);
		addCode(OpCode::JUMP);
		addCode(0);
		size_t jmp1 = this->compilingChunk->code.size() - 1;
		addCode(OpCode::POPN, a.getSize());
		DataType b = logic_and();
		if (b == ValueType::BOOL && a == b)
		{
			size_t here = this->compilingChunk->code.size() - 1;
			this->compilingChunk->code[jmp1] = here - jmp1;
		}
		else
			return typeError("Both operands of '||' must be a type of 'bool'.");
	}
	return a;
}

DataType Compiler::logic_and()
{
	DataType a = bit_or();
	if (a == ValueType::ERROR) return a;
	while (match(TokenType::AND))
	{
		addCode(OpCode::JUMP_NT);
		addCode(0);
		size_t jmp1 = this->compilingChunk->code.size() - 1;
		addCode(OpCode::POPN, a.getSize());
		DataType b = bit_or();
		if (b == ValueType::BOOL && a == b)
		{
			size_t here = this->compilingChunk->code.size() - 1;
			this->compilingChunk->code[jmp1] = here - jmp1;
		}
		else
			return typeError("Both operands of '&&' must be a type of 'bool'.");
	}
	return a;
}

DataType Compiler::bit_or()
{
	DataType a = bit_xor();
	if (a == ValueType::ERROR) return a;
	while (match(TokenType::BIT_OR))
	{
		DataType b = bit_xor();
		if (b == ValueType::INTEGER && a == b)
			addCode(OpCode::BIT_OR);
		else
			return typeError("Both operands of '|' must be a type of 'int'.");
	}
	return a;
}

DataType Compiler::bit_xor()
{
	DataType a = bit_and();
	if (a == ValueType::ERROR) return a;
	while (match(TokenType::BIT_XOR))
	{
		DataType b = bit_and();
		if (b == ValueType::INTEGER && a == b)
			addCode(OpCode::BIT_XOR);
		else
			return typeError("Both operands of '^' must be a type of 'int'.");
	}
	return a;
}

DataType Compiler::bit_and()
{
	DataType a = equality();
	if (a == ValueType::ERROR) return a;
	while (match(TokenType::BIT_AND))
	{
		DataType b = equality();
		if (b == ValueType::INTEGER && a == b)
			addCode(OpCode::BIT_AND);
		else
			return typeError("Both operands of '&' must be a type of 'int'.");
	}

	return a;
}

DataType Compiler::equality()
{
	DataType a = comparison();
	if (a == ValueType::ERROR) return a;
	DataType ret = a;
	while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL))
	{
		size_t pos = this->compilingChunk->code.size();
		TokenType type = consumed().type;
		DataType b = comparison();
		ret = ValueType::BOOL;
		if (a.type <= ValueType::BOOL && b.type <= ValueType::BOOL)
		{
			if (a.type >= ValueType::FLOAT || b.type >= ValueType::FLOAT)
			{
				emitCast(a, ValueType::DOUBLE, pos);
				emitCast(b, ValueType::DOUBLE);
				if (type == TokenType::EQUAL_EQUAL)
					addCode(OpCode::DIS_EQUAL);
				else
					addCode(OpCode::DNOT_EQUAL);
			}
			else
			{
				emitCast(a, ValueType::INTEGER, pos);
				emitCast(b, ValueType::INTEGER);
				if (type == TokenType::EQUAL_EQUAL)
					addCode(OpCode::IIS_EQUAL);
				else
					addCode(OpCode::INOT_EQUAL);
			}
		}
		else
			if (type == TokenType::EQUAL_EQUAL)
				return typeError("Both operands of '==' must be the same type.");
			else
				return typeError("Both operands of '!=' must be the same type.");
	}
	return ret;
}

DataType Compiler::comparison()
{
	DataType a = bitshift();
	if (a == ValueType::ERROR) return a;
	DataType ret = a;
	while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) || match(TokenType::GREAT) || match(TokenType::GREAT_EQUAL))
	{
		size_t pos = this->compilingChunk->code.size();
		TokenType type = consumed().type;
		DataType b = bitshift();
		if (a.type <= ValueType::BOOL && b.type <= ValueType::BOOL)
		{
			if (a.type >= ValueType::FLOAT || b.type >= ValueType::FLOAT)
			{
				emitCast(a, ValueType::DOUBLE, pos);
				emitCast(b, ValueType::DOUBLE);
				ret = ValueType::BOOL;
				switch (type)
				{
				case TokenType::LESS:
					addCode(OpCode::DLESS);
					break;
				case TokenType::LESS_EQUAL:
					addCode(OpCode::DLESS_EQUAL);
					break;
				case TokenType::GREAT:
					addCode(OpCode::DGREAT);
					break;
				case TokenType::GREAT_EQUAL:
					addCode(OpCode::DGREAT_EQUAL);
					break;
				default:
					// Unreachable
					break;
				}
			}
			else
			{
				emitCast(a, ValueType::INTEGER, pos);
				emitCast(b, ValueType::INTEGER);
				ret = ValueType::BOOL;
				switch (type)
				{
				case TokenType::LESS:
					addCode(OpCode::ILESS);
					break;
				case TokenType::LESS_EQUAL:
					addCode(OpCode::ILESS_EQUAL);
					break;
				case TokenType::GREAT:
					addCode(OpCode::IGREAT);
					break;
				case TokenType::GREAT_EQUAL:
					addCode(OpCode::IGREAT_EQUAL);
					break;
				default:
					// Unreachable
					break;
				}
			}
		}
		else
		{
			switch (type)
			{
			case TokenType::LESS:
				return typeError("Both operands of '<' must be the same type.");
				break;
			case TokenType::LESS_EQUAL:
				return typeError("Both operands of '<=' must be the same type.");
				break;
			case TokenType::GREAT:
				return typeError("Both operands of '>' must be the same type.");
				break;
			case TokenType::GREAT_EQUAL:
				return typeError("Both operands of '>=' must be the same type.");
				break;
			default:
				// Unreachable
				break;
			}
		}
	}

	return ret;
}

DataType Compiler::bitshift()
{
	DataType a = addition();
	if (a == ValueType::ERROR) return a;
	while (match(TokenType::BITSHIFT_LEFT) || match(TokenType::BITSHIFT_RIGHT))
	{
		TokenType type = consumed().type;
		DataType b = addition();
		if (b == ValueType::INTEGER && a == b)
		{
			if (type == TokenType::BITSHIFT_LEFT)
				addCode(OpCode::BITSHIFT_LEFT);
			else
				addCode(OpCode::BITSHIFT_RIGHT);
		}
		else
		{
			if (type == TokenType::BITSHIFT_LEFT)
				return typeError("Both operands of '<<' must be a type of 'int'.");
			else
				return typeError("Both operands of '>>' must be a type of 'int'.");
		}
	}
	return a;
}

DataType Compiler::addition()
{
	DataType a = multiplication();
	if (a == ValueType::ERROR) return a;
	DataType ret = a;
	while (match(TokenType::PLUS) || match(TokenType::MINUS))
	{
		size_t pos = this->compilingChunk->code.size();
		TokenType type = consumed().type;
		DataType b = multiplication();
		if (a == ValueType::INTEGER && b == ValueType::INTEGER)
		{
			if (type == TokenType::PLUS)
				addCode(OpCode::IADD);
			else
				addCode(OpCode::ISUB);
		}
		else if (a.type <= ValueType::FLOAT && b.type <= ValueType::FLOAT)
		{
			emitCast(a, ValueType::FLOAT, pos);
			emitCast(b, ValueType::FLOAT);

			if (type == TokenType::PLUS)
				addCode(OpCode::FADD);
			else
				addCode(OpCode::FSUB);
		}
		else if (a.type <= ValueType::DOUBLE && b.type <= ValueType::DOUBLE)
		{
			emitCast(a, ValueType::DOUBLE, pos);
			emitCast(b, ValueType::DOUBLE);

			if (type == TokenType::PLUS)
				addCode(OpCode::DADD);
			else
				addCode(OpCode::DSUB);
		}
		else
		{
			if (type == TokenType::PLUS)
				return typeError("Both operands of '+' must be a type of 'int', 'float' or 'double'.");
			else
				return typeError("Both operands of '-' must be a type of 'int', 'float' or 'double'.");
		}
		ret = (a.type > b.type) ? (a) : (b);
	}
	return ret;
}

DataType Compiler::multiplication()
{
	DataType a = prefix();
	if (a == ValueType::ERROR) return a;
	DataType ret = a;
	while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::MODULUS))
	{
		size_t pos = this->compilingChunk->code.size();
		TokenType type = consumed().type;
		DataType b = prefix();
		if (a == ValueType::INTEGER && b == ValueType::INTEGER)
		{
			if (type == TokenType::STAR)
				addCode(OpCode::IMUL);
			else if (type == TokenType::MODULUS)
				addCode(OpCode::MOD);
			else
				addCode(OpCode::IDIV);
		}
		else if (a.type <= ValueType::FLOAT && b.type <= ValueType::FLOAT)
		{
			emitCast(a, ValueType::FLOAT, pos);
			emitCast(b, ValueType::FLOAT);

			if (type == TokenType::STAR)
				addCode(OpCode::FMUL);
			else
				addCode(OpCode::FDIV);
		}
		else if (a.type <= ValueType::DOUBLE && b.type <= ValueType::DOUBLE)
		{
			emitCast(a, ValueType::DOUBLE, pos);
			emitCast(b, ValueType::DOUBLE);

			if (type == TokenType::STAR)
				addCode(OpCode::DMUL);
			else
				addCode(OpCode::DDIV);
		}
		else
		{
			if (type == TokenType::STAR)
				return typeError("Both operands of '*' must be a type of 'int', 'float' or 'double'.");
			else
				return typeError("Both operands of '/' must be a type of 'int', 'float' or 'double'.");
		}
		ret = (a.type > b.type) ? (a) : (b);
	}
	return ret;
}

DataType Compiler::prefix()
{
	if (match(TokenType::MINUS) || match(TokenType::PLUS) || match(TokenType::PLUS_PLUS) || match(TokenType::MINUS_MINUS))
	{
		TokenType type = consumed().type;
		DataType a = unary();
		if (a == ValueType::ERROR) return a;
		if (type == TokenType::MINUS)
		{
			if (a.intrinsicType == nullptr)
			{
				switch (a.type)
				{
				case ValueType::INTEGER:
					addCode(OpCode::INEG);
					return a;
				case ValueType::FLOAT:
					addCode(OpCode::FNEG);
					return a;
				case ValueType::DOUBLE:
					addCode(OpCode::DNEG);
					return a;
				default:
					return typeError("Operand of '-' must be a type of 'int', 'float' or 'double'.");
				}
			}
			else
			{
				return typeError("Operand of '-' must be a type of 'int', 'float' or 'double'.");
			}
		}
		else if (type == TokenType::PLUS)
		{
			if (a.type > ValueType::DOUBLE)
				return typeError("Operand of '+' must be a type of 'int', 'float' or 'double'.");
		}
		else if (type == TokenType::PLUS_PLUS)
		{
			if (a.intrinsicType == nullptr)
			{
				switch (a.type)
				{
				case ValueType::INTEGER:
					addCode(OpCode::IPRE_INC);
					return a;
				case ValueType::FLOAT:
					addCode(OpCode::FPRE_INC);
					return a;
				case ValueType::DOUBLE:
					addCode(OpCode::DPRE_INC);
					return a;
				default:
					return typeError("Operand of '++' must be a type of 'int', 'float' or 'double'.");
				}
			}
			else
			{
				return typeError("Operand of '++' must be a type of 'int', 'float' or 'double'.");
			}
		}
		else
		{
			if (a.intrinsicType == nullptr)
			{
				switch (a.type)
				{
				case ValueType::INTEGER:
					addCode(OpCode::IPRE_DEC);
					return a;
				case ValueType::FLOAT:
					addCode(OpCode::FPRE_DEC);
					return a;
				case ValueType::DOUBLE:
					addCode(OpCode::DPRE_DEC);
					return a;
				default:
					return typeError("Operand of '--' must be a type of 'int', 'float' or 'double'.");
				}
			}
			else
			{
				return typeError("Operand of '--' must be a type of 'int', 'float' or 'double'.");
			}
		}
		return a;
	}
	else
	{
		return unary();
	}
}

DataType Compiler::unary()
{
	if (match(TokenType::BANG) || match(TokenType::TILDE))
	{
		TokenType type = consumed().type;
		DataType a = unary();
		if (a == ValueType::ERROR) return a;
		if (type == TokenType::BANG)
		{
			if (a.type <= ValueType::BOOL)
			{
				emitCast(a, ValueType::BOOL);
				addCode(OpCode::LOGIC_NOT);
				return ValueType::BOOL;
			}
			else
				return typeError("Operand of '!' must be a type of 'bool'.");
		}
		else
		{
			if (a == ValueType::INTEGER)
			{
				addCode(OpCode::BIT_NOT);
				return a;
			}
			else
				return typeError("Operand of '~' must be a type of 'int'.");
		}
	}
	else if (matchCast())
	{
		DataType type = getCast();
		this->currentToken += 3;
		DataType a = unary();
		if (a == ValueType::ERROR) return a;
		emitCast(a, type);
		return type;
	}
	else
	{
		return postfix();
	}
}

DataType Compiler::postfix()
{
	DataType a = call();
	if (a == ValueType::ERROR) return a;

	if (match(TokenType::PLUS_PLUS))
	{
		if (a.intrinsicType == nullptr)
		{
			switch (a.type)
			{
			case ValueType::INTEGER:
				addCode(OpCode::IPOST_INC);
				return a;
			case ValueType::FLOAT:
				addCode(OpCode::FPOST_INC);
				return a;
			case ValueType::DOUBLE:
				addCode(OpCode::DPOST_INC);
				return a;
			default:
				return typeError("Operand of '++' must be a type of 'int', 'float' or 'double'.");
			}
		}
		else
		{
			return typeError("Operand of '++' must be a type of 'int', 'float' or 'double'.");
		}
	}
	else if (match(TokenType::MINUS_MINUS))
	{
		if (a.intrinsicType == nullptr)
		{
			switch (a.type)
			{
			case ValueType::INTEGER:
				addCode(OpCode::IPRE_DEC);
				return a;
			case ValueType::FLOAT:
				addCode(OpCode::FPRE_DEC);
				return a;
			case ValueType::DOUBLE:
				addCode(OpCode::DPRE_DEC);
				return a;
			default:
				return typeError("Operand of '--' must be a type of 'int', 'float' or 'double'.");
			}
		}
		else
		{
			return typeError("Operand of -- must be a type of 'int', 'float' or 'double'.");

		}
	}

	return a;
}

DataType Compiler::call()
{
	DataType a = primary();
	if (peek().type == TokenType::OPEN_PAREN)
	{
		if (a.type == ValueType::FUNCTION || a.type == ValueType::NATIVE)
		{
			while (match(TokenType::OPEN_PAREN) && (a.type == ValueType::FUNCTION || a.type == ValueType::NATIVE))
			{
				int argSize = 0;
				if (peek().type != TokenType::CLOSE_PAREN)
				{
					argSize += this->compileExpression().getSize();
				}
				while (!match(TokenType::CLOSE_PAREN))
				{
					consume(TokenType::COMMA, "Expect ',' between arguments.");
					argSize += this->compileExpression().getSize();
				}
				if (a.type == ValueType::FUNCTION)
					addCode(OpCode::CALL);
				else if (a.type == ValueType::NATIVE)
					addCode(OpCode::NATIVE_CALL);

				addCode(argSize);
				a = DataType(a.intrinsicType);
			}
			return a;
		}
		else
		{
			typeError("Only a function can be called.");
		}
	}
	return a;
}

DataType Compiler::primary()
{
	Token token = advance();

	switch (token.type)
	{
	case TokenType::TRUE:
	{
		addCode(OpCode::CONSTANT, sizeof(bool));
		addCode(this->compilingChunk->addConstant(Value(true).cloneData(), sizeof(bool)));
		return ValueType::BOOL;
	}

	case TokenType::FALSE:
		addCode(OpCode::CONSTANT, sizeof(bool));
		addCode(this->compilingChunk->addConstant(Value(false).cloneData(), sizeof(bool)));
		return ValueType::BOOL;

	case TokenType::NULL_TOKEN:
		addCode(OpCode::CONSTANT, 0);
		addCode(this->compilingChunk->addConstant(nullptr, 0));
		return ValueType::NULL_TYPE;

	case TokenType::INTEGER_LITERAL:
		addCode(OpCode::CONSTANT, sizeof(int32_t));
		addCode(this->compilingChunk->addConstant(Value(token.getInteger()).cloneData(), sizeof(int32_t)));
		return ValueType::INTEGER;

	case TokenType::FLOAT_LITERAL:
		addCode(OpCode::CONSTANT, sizeof(float));
		addCode(this->compilingChunk->addConstant(Value(token.getFloat()).cloneData(), sizeof(float)));
		return ValueType::FLOAT;

	case TokenType::DOUBLE_LITERAL:
		addCode(OpCode::CONSTANT, sizeof(double));
		addCode(this->compilingChunk->addConstant(Value(token.getDouble()).cloneData(), sizeof(double)));
		return ValueType::DOUBLE;

	case TokenType::STRING_LITERAL:
		addCode(OpCode::CONSTANT, sizeof(char*));
		addCode(this->compilingChunk->addConstant(StrValue(token.getString()).cloneData(), sizeof(char*)));
		return ValueType::STRING;

	case TokenType::CHAR_LITERAL:
		addCode(OpCode::CONSTANT, sizeof(char));
		addCode(this->compilingChunk->addConstant(Value(token.getChar()).cloneData(), sizeof(char)));
		return ValueType::CHAR;

	case TokenType::OPEN_PAREN:
	{
		DataType a = compileExpression();
		if (match(TokenType::CLOSE_PAREN))
			return a;
		else
			return typeError("Expect ')' after a grouping expression.");
	}

	case TokenType::IDENTIFIER:
	{
		int slot = -1;
		for (int i = this->locals.size() - 1; i >= frame; i--)
		{
			if (this->locals[i].name == token.getString())
			{
				slot = i;
				break;
			}
		}

		// Not Local
		if (slot == -1)
		{
			auto it = globals.find(token.getString());
			// Global
			if (it != globals.end())
			{
				addCode(OpCode::GET_GLOBAL, globals[token.getString()].first->type.getSize());
				addCode(getGlobal(token.getString()));
				return globals[token.getString()].first->type;
			}
			// DNE
			else
			{
				return typeError("Identifier is not defined.");
			}
		}
		// Local
		else
		{
			addCode(OpCode::GET_LOCAL, locals[slot].startPos, locals[slot].type.getSize());
			return this->locals[slot].type;
		}
	}

	default:
		return ValueType::ERROR;
	}

	return DataType();
}

bool Compiler::match(TokenType type)
{
	TokenType next = tokens[currentToken].type;
	if (next == type)
	{
		currentToken++;
		return true;
	}

	return false;
}

bool Compiler::matchCast()
{
	return tokens[currentToken].type == TokenType::OPEN_PAREN && getCast() != ValueType::ERROR && tokens[currentToken + 2].type == TokenType::CLOSE_PAREN;
}

DataType Compiler::getCast()
{
	switch (tokens[currentToken + 1].type)
	{
	case TokenType::INT:
		return ValueType::INTEGER;
	case TokenType::FLOAT:
		return ValueType::FLOAT;
	case TokenType::DOUBLE:
		return ValueType::DOUBLE;
	case TokenType::CHAR:
		return ValueType::CHAR;
	case TokenType::STRING:
		return ValueType::STRING;
	case TokenType::BOOL:
		return ValueType::BOOL;
	case TokenType::VOID:
		return ValueType::VOID;
	default:
		return ValueType::ERROR;
	}
}

void Compiler::addCode(OpCode code)
{
	this->compilingChunk->code.push_back((uint8_t)code);
}

void Compiler::addCode(OpCode code1, uint8_t code2)
{
	this->compilingChunk->code.push_back((uint8_t)code1);
	this->compilingChunk->code.push_back(code2);
}

void Compiler::addCode(OpCode code1, uint8_t code2, uint8_t code3)
{
	this->compilingChunk->code.push_back((uint8_t)code1);
	this->compilingChunk->code.push_back(code2);
	this->compilingChunk->code.push_back(code3);
}

void Compiler::addCode(uint8_t code)
{
	this->compilingChunk->code.push_back(code);
}

void Compiler::emitCast(DataType from, DataType to)
{
	// TODO: add support for build in types
	if (to != from)
	{
		addCode(OpCode::CAST);
		addCode((uint8_t)from.type);
		addCode((uint8_t)to.type);
	}
}

void Compiler::emitCast(DataType from, DataType to, size_t pos)
{
	if (to != from)
	{
		auto& code = this->compilingChunk->code;
		code.insert(code.begin() + pos, (uint8_t)OpCode::CAST);
		code.insert(code.begin() + pos + 1, (uint8_t)from.type);
		code.insert(code.begin() + pos + 2, (uint8_t)to.type);
	}
}

inline DataType Compiler::typeError(const char* message) const
{
	std::cout << "At " << tokens[currentToken - 1] << message << std::endl;
	return ValueType::ERROR;
}

inline void Compiler::error(const char* message)
{
	std::cout << message << std::endl;
	this->panic();
}

inline void Compiler::errorAtToken(const char* message)
{
	std::cout << "[line" << tokens[currentToken].line << "] Error" << message << std::endl;
	this->panic();
}

void Compiler::panic()
{
	TokenType type = tokens[currentToken].type;
	while (type != TokenType::EOF_TOKEN)
	{
		if (type == TokenType::SEMI_COLON)
		{
			advance();
			return;
		}

		switch (type)
		{
		case TokenType::USING:
		case TokenType::NAMESPACE:
		case TokenType::CLASS:
		case TokenType::IF:
		case TokenType::INT:
		case TokenType::FLOAT:
		case TokenType::DOUBLE:
		case TokenType::CHAR:
		case TokenType::STRING:
		case TokenType::BOOL:
		case TokenType::VOID:
		case TokenType::WHILE:
		case TokenType::FOR:
		case TokenType::RETURN:
			return;

		case TokenType::IDENTIFIER:
			// TODO: add user defined types
			// if (isTypeName(tokens[currentToken]))
			return;

		default:
			break;
		}

		type = advance().type;
	}
}
