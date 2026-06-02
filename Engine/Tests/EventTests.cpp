#include "Event/Event.h"
#include <gtest/gtest.h>

using namespace RPE;

TEST(EventTests, EventMustDeliverCorrectValue)
{
    Event<int> event;
    int receivedValue = 0;

    event.subscribe([&receivedValue](int value) { receivedValue = value; });

    event.invoke(42);

    EXPECT_EQ(receivedValue, 42);
}

TEST(EventTests, EventMustDeliverMultipleValues)
{
    Event<int, float> event;
    int receivedInt = 0;
    float receivedFloat = 0.0f;

    event.subscribe(
        [&receivedInt, &receivedFloat](int i, float f)
        {
            receivedInt = i;
            receivedFloat = f;
        });

    event.invoke(10, 3.14f);

    EXPECT_EQ(receivedInt, 10);
    EXPECT_FLOAT_EQ(receivedFloat, 3.14f);
}

TEST(EventTests, MultipleSubscribersMustAllReceiveEvent)
{
    Event<int> event;
    int value1 = 0;
    int value2 = 0;
    int value3 = 0;

    event.subscribe([&value1](int v) { value1 = v; });
    event.subscribe([&value2](int v) { value2 = v; });
    event.subscribe([&value3](int v) { value3 = v; });

    event.invoke(100);

    EXPECT_EQ(value1, 100);
    EXPECT_EQ(value2, 100);
    EXPECT_EQ(value3, 100);
}

TEST(EventTests, SubscribersCanBeAddedAfterInvoke)
{
    Event<int> event;
    int receivedValue = 0;

    event.invoke(42);

    event.subscribe([&receivedValue](int v) { receivedValue = v; });

    event.invoke(100);

    EXPECT_EQ(receivedValue, 100);
}

TEST(EventTests, LambdaCapturesWorkCorrectly)
{
    Event<std::string> event;
    std::string result;
    std::string prefix = "Hello ";

    event.subscribe([&result, prefix](const std::string& name) { result = prefix + name; });

    event.invoke("World");

    EXPECT_EQ(result, "Hello World");
}

TEST(EventTests, EventWithConstReference)
{
    Event<const std::string&> event;
    std::string received;

    event.subscribe([&received](const std::string& str) { received = str; });

    std::string testString = "Test Message";
    event.invoke(testString);

    EXPECT_EQ(received, testString);
}

TEST(EventTests, MultipleInvokesWorkCorrectly)
{
    Event<int> event;
    std::vector<int> receivedValues;

    event.subscribe([&receivedValues](int v) { receivedValues.push_back(v); });

    event.invoke(1);
    event.invoke(2);
    event.invoke(3);

    ASSERT_EQ(receivedValues.size(), 3);
    EXPECT_EQ(receivedValues[0], 1);
    EXPECT_EQ(receivedValues[1], 2);
    EXPECT_EQ(receivedValues[2], 3);
}

TEST(EventTests, EventWithInputEvent)
{
    Event<const InputEvent&> event;
    InputEvent receivedEvent;
    receivedEvent.type = EventType::WindowResize;

    event.subscribe([&receivedEvent](const InputEvent& e) { receivedEvent = e; });

    InputEvent testEvent;
    testEvent.type = EventType::KeyPress;
    testEvent.data = KeyData{87, 17, 1, 0};  // Key W, press

    event.invoke(testEvent);

    EXPECT_EQ(receivedEvent.type, EventType::KeyPress);
    EXPECT_TRUE(std::holds_alternative<KeyData>(receivedEvent.data));

    if (std::holds_alternative<KeyData>(receivedEvent.data))
    {
        const auto& keyData = std::get<KeyData>(receivedEvent.data);
        EXPECT_EQ(keyData.key, 87);
        EXPECT_EQ(keyData.action, 1);
    }
}