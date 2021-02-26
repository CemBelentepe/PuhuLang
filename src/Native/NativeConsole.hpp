#pragma once
#include "Native.hpp"
#include <memory>
#include <vector>

class NativePrint : public NativeFunc
{
public:
    explicit NativePrint()
        : NativeFunc(
              std::make_shared<TypeFunction>(
                  std::make_shared<TypePrimitive>(TypePrimitive::PrimitiveTag::VOID),
                  std::vector<std::shared_ptr<Type>>({std::make_shared<TypeString>()})),
              {"phc"},
              "print")
    {
    }
    Value call(Interpreter* interpreter, std::vector<Value> args) override;
};

class NativeInput : public NativeFunc
{
public:
    explicit NativeInput()
        : NativeFunc(
              std::make_shared<TypeFunction>(
                  std::make_shared<TypeString>(),
                  std::vector<std::shared_ptr<Type>>()),
              {"phc"},
              "input")
    {
    }
    Value call(Interpreter* interpreter, std::vector<Value> args) override;
};
