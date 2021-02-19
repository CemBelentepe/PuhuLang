#include "Callable.hpp"

PuhuFunction::PuhuFunction(DeclFunc* func)
    : func(func)
{
}

Value PuhuFunction::call(Interpreter* interpreter, std::vector<Value> args)
{
    const std::vector<std::shared_ptr<Type>>& param_types = std::dynamic_pointer_cast<TypeFunction>(func->type)->param_types;

    for (size_t i = 0; i < func->param_names.size(); i++)
    {
        interpreter->getCurrentEnviroment()->addVariable(Interpreter::RunTimeVariable(func->param_names[i], param_types[i], true, args[i]));
    }

    try
    {
        for (auto& s : func->body)
        {
            s->accept(interpreter);
        }
    }
    catch (const Value& retVal)
    {
        return retVal;
    }
    return Value();
}

std::vector<std::shared_ptr<Type>> PuhuFunction::arity()
{
    return std::dynamic_pointer_cast<TypeFunction>(func->type)->param_types;
}

std::string PuhuFunction::name()
{
    return std::string(func->name.lexeme);
}

std::shared_ptr<Type> PuhuFunction::getType() 
{
    return func->type;
}
