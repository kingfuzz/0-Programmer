/**
 * @class ThreadSafeMessageQueue
 * @brief ThreadSafeMessageQueue implements a message queue on top of the JUCE AbstractFifo
 *
 * This class allows you to send messages between a two processes. NOTE: please be aware that
 * the implementation is limited to one (1!) producer and one (1!) consumer.
 * 
 * This is very useful to communicate stuff from the Editor to the Processor.
 *
 */


#pragma once

#include <juce_core/juce_core.h>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cstring> // For memcpy


struct GuiMessage
{
    enum Type
    {
        cc
    };
    Type type;
    int value1;
    int value2;
    int value3;
};

class ThreadSafeMessageQueue : public juce::AbstractFifo
{
public:
    ThreadSafeMessageQueue(int capacity) : AbstractFifo(capacity), buffer_(static_cast<size_t>(capacity) * sizeof(GuiMessage)) {}

    bool push(const GuiMessage& message)
    {
        //DBG("ThreadSafeMessageQueue::push: getFreeSpace() " << getFreeSpace());
        if (getFreeSpace() > 0)
        {
            // We'll always write one message at a time
            int numItems = 1;
            const auto scope = write (numItems);
            size_t numBytesToWrite = sizeof(GuiMessage);

            // Copy the message into the buffer
            if (scope.blockSize1 > 0)
            {
                auto* dest = static_cast<GuiMessage*>(buffer_.getData());
                std::memcpy(&dest[scope.startIndex1], &message, numBytesToWrite);
            }
            jassert(scope.blockSize2 == 0); // We should never have a second block for a single message

            return true;
        }
        return false;
    }

    bool pop(GuiMessage& message)
    {
        if (getNumReady() > 0)
        {
            // We'll always read one message at a time
            int numItems = 1;
            const auto scope = read (numItems);
            size_t numBytesToRead = sizeof(GuiMessage);

            if (scope.blockSize1 > 0)
            {
                auto* src = static_cast<GuiMessage*>(buffer_.getData());
                std::memcpy(&message, &src[scope.startIndex1], numBytesToRead);
            }
            jassert(scope.blockSize2 == 0); // We should never have a second block for a single message
            return true;
        }
        return false;
    }

    int getNumReady() const
    {
        return AbstractFifo::getNumReady();
    }

private:
    juce::MemoryBlock buffer_;
    std::mutex mutex_;
    std::condition_variable dataAvailable_;
};
