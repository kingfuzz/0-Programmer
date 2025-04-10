/**
 * @class Parameters
 * @brief A thread-safe class for managing a collection of parameters.
 *
 * The Parameters class allows you to define, modify, retrieve, and delete
 * parameters. Each parameter is identified by a unique name and contains
 * metadata such as its control change (cc) value, current value, minimum
 * and maximum values, and an updated flag.
 *
 * This class uses a mutex to ensure thread safety for all operations. NOTE: 
 * mutexes may block a realtime thread, so use with caution in a realtime context!
 */
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

    /**
     * @brief Adds a new parameter to the collection.
     * 
     * @param name The name of the parameter.
     * @param cc The control change (CC) number associated with the parameter.
     * @param value The initial value of the parameter.
     * @param minValue The minimum allowable value for the parameter.
     * @param maxValue The maximum allowable value for the parameter.
     * 
     * @throws std::runtime_error If a parameter with the same name already exists.
     */
    void addParameter(const std::string& name, int cc, int value, int minValue, int maxValue)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (parameters.find(name) != parameters.end())
        {
            throw std::runtime_error("Parameter already exists");
        }
        parameters[name] = {cc, value, minValue, maxValue, false, name};
    }

    /**
    * @brief Sets the value of an existing parameter. Will set the update flag, 
    * if the value is different from the current one.
    * 
    * @param name The name of the parameter to update.
    * @param value The new value to set for the parameter.
    * 
    * @throws std::runtime_error If the parameter with the given name is not found.
    */
    void setParameter(const std::string& name, int value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = parameters.find(name);
        if (it == parameters.end())
        {
            throw std::runtime_error("Parameter not found");
        }
        if ( value != it->second.value )
        {
            it->second.value = value;
            it->second.updated = true;
        }
    }
    
    /**
     * @brief Retrieves all parameters in the collection.
     * 
     * @return std::unordered_map<std::string, Parameter> A map of all parameters.
     */
    std::unordered_map<std::string, Parameter> getAllParameters() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return parameters;
    }

    /**
     * @brief Retrieves the full details of a parameter.
     * 
     * @param name The name of the parameter to retrieve.
     * @return Parameter The details of the parameter.
     * 
     * @throws std::runtime_error If the parameter with the given name is not found.
     */
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

    /**
     * @brief Retrieves the current value of a parameter.
     * 
     * @param name The name of the parameter to retrieve the value for.
     * @return int The current value of the parameter.
     * 
     * @throws std::runtime_error If the parameter with the given name is not found.
     */
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

    /**
     * @brief Checks if a parameter has been updated since the last read.
     * 
     * @note This function resets the updated status of the parameter to false.
     * 
     * @param name The name of the parameter to check.
     * @return bool True if the parameter was updated, false otherwise.
     * 
     * @throws std::runtime_error If the parameter with the given name is not found.
     */
    bool isParameterUpdated(const std::string& name)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = parameters.find(name);
        if (it == parameters.end())
        {
            throw std::runtime_error("Parameter not found");
        }
        bool updated = it->second.updated;
        it->second.updated = false;
        return updated;
    }

    /**
    * @brief Deletes a parameter from the collection.
    * 
    * @param name The name of the parameter to delete.
    * 
    * @throws std::runtime_error If the parameter with the given name is not found.
    */
    void deleteParameter(const std::string& name)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = parameters.find(name);
        if (it == parameters.end())
        {
            throw std::runtime_error("Parameter not found");
        }
        parameters.erase(it);
    }

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, Parameter> parameters;
};