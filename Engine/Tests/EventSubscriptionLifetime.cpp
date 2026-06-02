#include "Event/Event.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace RPE;

// ======================== Тестирование времени жизни подписки ========================

class EventSource
{
public:
    Event<int> onData;

    void fireEvent(int value)
    {
        lastValue = value;
        onData.invoke(value);
    }

    int getLastValue() const { return lastValue; }

private:
    int lastValue = 0;
};

class Subscriber
{
public:
    void onEventReceived(int value)
    {
        lastReceivedValue = value;
        receiveCount++;
        receivedValues.push_back(value);
    }

    int getLastReceivedValue() const { return lastReceivedValue; }
    int getReceiveCount() const { return receiveCount; }
    const std::vector<int>& getReceivedValues() const { return receivedValues; }
    void reset()
    {
        lastReceivedValue = 0;
        receiveCount = 0;
        receivedValues.clear();
    }

private:
    int lastReceivedValue = 0;
    int receiveCount = 0;
    std::vector<int> receivedValues;
};

// Тест 1: Подписчик НЕ получает события ДО подписки
TEST(EventSubscriptionLifetimeTest, NoEventsBeforeSubscription)
{
    EventSource source;
    Subscriber sub;

    source.fireEvent(10);
    source.fireEvent(20);
    source.fireEvent(30);

    auto id = source.onData.subscribe([&sub](int value) { sub.onEventReceived(value); });

    source.fireEvent(40);

    EXPECT_EQ(sub.getReceiveCount(), 1);
    EXPECT_EQ(sub.getLastReceivedValue(), 40);

    source.onData.unsubscribe(id);
}

// Тест 2: Подписчик НЕ получает события ПОСЛЕ отписки
TEST(EventSubscriptionLifetimeTest, NoEventsAfterUnsubscription)
{
    EventSource source;
    Subscriber sub;

    auto id = source.onData.subscribe([&sub](int value) { sub.onEventReceived(value); });

    source.fireEvent(10);
    EXPECT_EQ(sub.getReceiveCount(), 1);

    source.onData.unsubscribe(id);

    source.fireEvent(20);
    source.fireEvent(30);

    EXPECT_EQ(sub.getReceiveCount(), 1);
    EXPECT_EQ(sub.getLastReceivedValue(), 10);
}

// Тест 3: Подписчик не должен получать события до подписки, даже если их много
TEST(EventSubscriptionLifetimeTest, NoEventsBeforeSubscriptionEvenMany)
{
    EventSource source;
    Subscriber sub;

    for (int i = 1; i <= 100; ++i)
    {
        source.fireEvent(i);
    }

    auto id = source.onData.subscribe([&sub](int value) { sub.onEventReceived(value); });

    source.fireEvent(999);

    EXPECT_EQ(sub.getReceiveCount(), 1);
    EXPECT_EQ(sub.getLastReceivedValue(), 999);

    source.onData.unsubscribe(id);
}

// Тест 4: ОДИН раз даже если событие несколько раз запускалось после подписки
TEST(EventSubscriptionLifetimeTest, SubscribeOnceThenMultipleFires)
{
    EventSource source;
    Subscriber sub;

    auto id = source.onData.subscribe([&sub](int value) { sub.onEventReceived(value); });

    for (int i = 1; i <= 10; ++i)
    {
        source.fireEvent(i * 10);
    }

    EXPECT_EQ(sub.getReceiveCount(), 10);

    source.onData.unsubscribe(id);
}

// Тест 5: Несколько подписчиков - независимые отписки
TEST(EventSubscriptionLifetimeTest, MultipleSubscribersIndependentUnsubscription)
{
    EventSource source;
    Subscriber sub1, sub2, sub3;

    auto id1 = source.onData.subscribe([&sub1](int v) { sub1.onEventReceived(v); });
    auto id2 = source.onData.subscribe([&sub2](int v) { sub2.onEventReceived(v); });
    auto id3 = source.onData.subscribe([&sub3](int v) { sub3.onEventReceived(v); });

    source.fireEvent(10);
    EXPECT_EQ(sub1.getReceiveCount(), 1);
    EXPECT_EQ(sub2.getReceiveCount(), 1);
    EXPECT_EQ(sub3.getReceiveCount(), 1);

    source.onData.unsubscribe(id2);

    source.fireEvent(20);
    EXPECT_EQ(sub1.getReceiveCount(), 2);
    EXPECT_EQ(sub2.getReceiveCount(), 1);
    EXPECT_EQ(sub3.getReceiveCount(), 2);

    source.onData.unsubscribe(id1);

    source.fireEvent(30);
    EXPECT_EQ(sub1.getReceiveCount(), 2);
    EXPECT_EQ(sub2.getReceiveCount(), 1);
    EXPECT_EQ(sub3.getReceiveCount(), 3);

    source.onData.unsubscribe(id3);

    source.fireEvent(40);
    EXPECT_EQ(sub1.getReceiveCount(), 2);
    EXPECT_EQ(sub2.getReceiveCount(), 1);
    EXPECT_EQ(sub3.getReceiveCount(), 3);
}

// Тест 6: Отписка невалидного ID не должна ломать другие подписки
TEST(EventSubscriptionLifetimeTest, UnsubscribeInvalidIdDoesNotAffectOthers)
{
    EventSource source;
    Subscriber sub1, sub2;

    auto id1 = source.onData.subscribe([&sub1](int v) { sub1.onEventReceived(v); });
    auto id2 = source.onData.subscribe([&sub2](int v) { sub2.onEventReceived(v); });

    source.onData.unsubscribe(99999);

    source.fireEvent(100);

    EXPECT_EQ(sub1.getReceiveCount(), 1);
    EXPECT_EQ(sub2.getReceiveCount(), 1);

    source.onData.unsubscribe(id1);
    source.onData.unsubscribe(id2);
}

// Тест 7: Полная очистка всех подписок
TEST(EventSubscriptionLifetimeTest, ClearAllSubscriptions)
{
    EventSource source;
    Subscriber sub1, sub2, sub3;

    source.onData.subscribe([&sub1](int v) { sub1.onEventReceived(v); });
    source.onData.subscribe([&sub2](int v) { sub2.onEventReceived(v); });
    source.onData.subscribe([&sub3](int v) { sub3.onEventReceived(v); });

    source.fireEvent(10);
    EXPECT_EQ(sub1.getReceiveCount(), 1);
    EXPECT_EQ(sub2.getReceiveCount(), 1);
    EXPECT_EQ(sub3.getReceiveCount(), 1);

    source.onData.clear();

    source.fireEvent(20);

    EXPECT_EQ(sub1.getReceiveCount(), 1);
    EXPECT_EQ(sub2.getReceiveCount(), 1);
    EXPECT_EQ(sub3.getReceiveCount(), 1);
}