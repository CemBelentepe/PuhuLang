#pragma once
#include "../Callable.hpp"
#include "../Type.hpp"
#include "../Token.hpp"

class Interpreter;
class DeclParser;

class NativeFunc : public Callable
{
private:
    static std::vector<std::shared_ptr<NativeFunc>> natives;

    std::shared_ptr<Type> type;
    std::vector<std::string> address;
    std::string m_name;
    Token token;
public:
    explicit NativeFunc(std::shared_ptr<Type> type, const std::vector<std::string>& address, const std::string& name);

    std::vector<std::shared_ptr<Type>> arity() override
    {
        return std::dynamic_pointer_cast<TypeFunction>(type)->param_types;
    }
    std::string name() override
    {
        return m_name;
    }
    std::shared_ptr<Type> getType() override
    {
        return type;
    }
    std::vector<std::string> getNamespace() override
    {
        return address;
    }

    Token tokenName()
    {
        return token;
    }

    static void Init();
    static void InitTo(DeclParser* parser);
    static void InitTo(Interpreter* interpreter);
};
