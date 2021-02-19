#pragma once
#include <string>
#include <variant>
#include <memory>

class Type;
class Callable;
class Data
{
public:
    std::variant<bool, char, int, float, double, std::string, Data*, std::shared_ptr<Callable>> data;

    Data() = default;

    template <typename T>
    explicit Data(T data)
        : data(std::move(data))
    {
    }
};

class Value
{
public:
    std::shared_ptr<Type> type;
    Data data;

    explicit Value();
    explicit Value(bool data);
    explicit Value(char data);
    explicit Value(int data);
    explicit Value(float data);
    explicit Value(double data);
    explicit Value(const std::string& data);
    explicit Value(std::shared_ptr<Callable> data);

    // TODO add ptr here!

    friend std::ostream& operator<<(std::ostream& os, const Value& val);
};
