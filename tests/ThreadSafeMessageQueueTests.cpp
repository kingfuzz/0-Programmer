#include <catch2/catch_test_macros.hpp>
#include "../source/ThreadSafeMessageQueue.h"

TEST_CASE("ThreadSafeMessageQueue functionality", "[ThreadSafeMessageQueue]")
{
    // NOTE: Actual Capacity for AbstractFifo is capacity-1!
    constexpr int capacity = 10;
    ThreadSafeMessageQueue queue(capacity);

    SECTION("Push and pop single message")
    {
        GuiMessage messageToPush{GuiMessage::cc, 1, 2, 3};
        GuiMessage messagePopped;

        REQUIRE(queue.push(messageToPush)); // Push should succeed
        REQUIRE(queue.getNumReady() == 1); // One message should be ready

        REQUIRE(queue.pop(messagePopped)); // Pop should succeed
        REQUIRE(queue.getNumReady() == 0); // No messages should be left

        // Verify the message content
        REQUIRE(messagePopped.type == messageToPush.type);
        REQUIRE(messagePopped.value1 == messageToPush.value1);
        REQUIRE(messagePopped.value2 == messageToPush.value2);
        REQUIRE(messagePopped.value3 == messageToPush.value3);
    }

    SECTION("Push until full")
    {
        GuiMessage messageToPush{GuiMessage::cc, 1, 2, 3};

        // NOTE: Actual Capacity for AbstractFifo is capacity-1!
        for (int i = 0; i < capacity-1; ++i)
        {
            REQUIRE(queue.push(messageToPush)); // Push should succeed until full
        }

        REQUIRE(queue.getNumReady() == capacity-1); // Queue should be full
        REQUIRE_FALSE(queue.push(messageToPush)); // Push should fail when full
    }

    SECTION("Pop from empty queue")
    {
        GuiMessage messagePopped;
        REQUIRE_FALSE(queue.pop(messagePopped)); // Pop should fail when queue is empty
    }

    SECTION("Push and pop multiple messages")
    {
        GuiMessage messageToPush1{GuiMessage::cc, 1, 2, 3};
        GuiMessage messageToPush2{GuiMessage::cc, 4, 5, 6};
        GuiMessage messagePopped;

        REQUIRE(queue.push(messageToPush1)); // Push first message
        REQUIRE(queue.push(messageToPush2)); // Push second message
        REQUIRE(queue.getNumReady() == 2);   // Two messages should be ready

        REQUIRE(queue.pop(messagePopped)); // Pop first message
        REQUIRE(messagePopped.type == messageToPush1.type);
        REQUIRE(messagePopped.value1 == messageToPush1.value1);
        REQUIRE(messagePopped.value2 == messageToPush1.value2);
        REQUIRE(messagePopped.value3 == messageToPush1.value3);

        REQUIRE(queue.pop(messagePopped)); // Pop second message
        REQUIRE(messagePopped.type == messageToPush2.type);
        REQUIRE(messagePopped.value1 == messageToPush2.value1);
        REQUIRE(messagePopped.value2 == messageToPush2.value2);
        REQUIRE(messagePopped.value3 == messageToPush2.value3);

        REQUIRE(queue.getNumReady() == 0); // Queue should be empty
    }

    SECTION("Concurrent push and pop")
    {
        GuiMessage messageToPush{GuiMessage::cc, 1, 2, 3};
        std::atomic<bool> producerDone{false};
        std::atomic<int> messagesPopped{0};

        // Producer thread
        std::thread producer([&]() {
            for (int i = 0; i < capacity; ++i)
            {
                while (!queue.push(messageToPush)) {} // Retry until successful
            }
            producerDone = true;
        });

        // Consumer thread
        std::thread consumer([&]() {
            GuiMessage messagePopped;
            while (!producerDone || queue.getNumReady() > 0)
            {
                if (queue.pop(messagePopped))
                {
                    ++messagesPopped;
                }
            }
        });

        producer.join();
        consumer.join();

        REQUIRE(messagesPopped == capacity); // All messages should be popped
    }
}
