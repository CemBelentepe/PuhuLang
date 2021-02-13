#pragma once
#include "Type.hpp"

#include <string>
#include <variant>

class Data
{
public:
    std::variant<bool, char, int, float, double, std::string, Data*> data;

    Data() = default;

    template <typename T>
    explicit Data(T data)
        : data(data)
    {
    }
};

class Value
{
public:
    std::shared_ptr<Type> type;
    Data data;

    Value() = default;

    explicit Value(bool data)
        : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::BOOL))
    {
    }
    explicit Value(char data)
        : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::CHAR))
    {
    }
    explicit Value(int data)
        : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::INT))
    {
    }
    explicit Value(float data)
        : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::FLOAT))
    {
    }
    explicit Value(double data)
        : data(Data(data)), type(std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::DOUBLE))
    {
    }
    explicit Value(const std::string& data)
        : data(Data(data)), type(std::make_shared<TypeString>())
    {
    }

    // TODO add ptr here!

    friend std::ostream& operator<<(std::ostream& os, const Value& val)
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
        else if(val.type->tag == Type::Tag::STRING)
        {
            os << std::get<std::string>(val.data.data);
        }
        else if(val.type->tag == Type::Tag::POINTER)
        {
            os << std::get<Data*>(val.data.data);
        }
        return os;
    }
};
