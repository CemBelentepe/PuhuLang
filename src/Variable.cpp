#include "Variable.hpp"
#include "Type.hpp"

Variable::Variable(const Token& name, const std::shared_ptr<Type>& type, bool initialized)
    : name(name), type(type), initialized(initialized)
{
}
