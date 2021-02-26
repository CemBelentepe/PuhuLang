#include "Native.hpp"
#include "NativeConsole.hpp"

#include "../DeclParser.hpp"
#include "../Interpreter.hpp"

std::vector<std::shared_ptr<NativeFunc>> NativeFunc::natives;

NativeFunc::NativeFunc(std::shared_ptr<Type> type, const std::vector<std::string>& address, const std::string& name)
    : type(type), address(address), m_name(name), token(TokenType::IDENTIFIER, -1, -1, m_name)
{
}

void NativeFunc::Init() 
{
    if (natives.size() == 0)
    {
        natives.push_back(std::make_shared<NativePrint>());
        natives.push_back(std::make_shared<NativeInput>());
    }
}

void NativeFunc::InitTo(DeclParser* parser)
{
    for (auto& ptr : natives)
    {
        parser->addNativeCallable(ptr.get());
    }
}

void NativeFunc::InitTo(Interpreter* interpreter)
{
    for (auto& ptr : natives)
    {
        interpreter->addNativeCallable(ptr);
    }
}