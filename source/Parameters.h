#pragma once
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <mutex>

class Parameters
{
public:
    struct Parameter
    {
        int cc;
        int value;
        int minValue;
        int maxValue;
        bool updated;
        std::string name;
    };

    Parameters() = default;

    void addParameter(const std::string& name, int cc, int value, int minValue, int maxValue)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (parameters.find(name) != parameters.end())
        {
            throw std::runtime_error("Parameter already exists");
        }
        parameters[name] = {cc, value, minValue, maxValue, false, name};
    }

    void setParameter(const std::string& name, int value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = parameters.find(name);
        if (it == parameters.end())
        {
            throw std::runtime_error("Parameter not found");
        }
        it->second.value = value;
        it->second.updated = true;
    }

    Parameter getParameter(const std::string& name) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = parameters.find(name);
        if (it != parameters.end())
        {
            return it->second;
        }
        throw std::runtime_error("Parameter not found");
    }

    int getParameterValue(const std::string& name) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = parameters.find(name);
        if (it == parameters.end())
        {
            throw std::runtime_error("Parameter not found");
        }
        return it->second.value;
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, Parameter> parameters;
};