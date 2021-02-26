#include "Value.hpp"
#include "Callable.hpp"
#include "Type.hpp"

Value::Value()
    : data(Data(nullptr)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::VOID))
{
}
Value::Value(bool data)
    : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::BOOL))
{
}
Value::Value(char data)
    : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::CHAR))
{
}
Value::Value(int data)
    : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::INT))
{
}
Value::Value(float data)
    : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::FLOAT))
{
}
Value::Value(double data)
    : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::DOUBLE))
{
}
Value::Value(const std::string& data)
    : data(Data(data)), type(std::make_shared<TypeString>())
{
}
Value::Value(std::shared_ptr<Callable> data_)
    : data(Data(data_)), type(data_->getType())
{
}

std::ostream& operator<<(std::ostream& os, const Value& val)
{
    if (val.type->tag == Type::Tag::PRIMITIVE)
    {
        auto orig = dynamic_cast<TypePrimitive*>(val.type.get());
        switch (orig->special_tag)
        {
        case TypePrimitive::PrimitiveTag::BOOL:
            os << (std::get<bool>(val.data.data) ? "true" : "false");
            break;
        case TypePrimitive::PrimitiveTag::CHAR:
            os << std::get<char>(val.data.data);
            break;
        case TypePrimitive::PrimitiveTag::INT:
            os << std::get<int>(val.data.data);
            break;
        case TypePrimitive::PrimitiveTag::FLOAT:
            os << std::get<float>(val.data.data) << "f";
            break;
        case TypePrimitive::PrimitiveTag::DOUBLE:
            os << std::get<double>(val.data.data);
            break;
        default:
            os << "[void]";
            break;
        }
    }
    else if (val.type->tag == Type::Tag::STRING)
    {
        os << std::get<std::string>(val.data.data);
    }
    else if (val.type->tag == Type::Tag::POINTER)
    {
        Value* ptr = std::get<Value*>(val.data.data);
        os << "*" << *ptr << "[" << ptr << "]";
    }
    else if (val.type->tag == Type::Tag::FUNCTION)
    {
        os << std::get<std::shared_ptr<Callable>>(val.data.data)->name();
    }
    return os;
}
