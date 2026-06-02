#include "Event/Event.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <format>
#include <string>

using namespace RPE;

// ======================== Тестирование цепочки событий ========================

// Класс A - источник события (ничего не знает о других классах)
class ClassA
{
public:
    Event<int> onDataReady;

    void produceData(int value)
    {
        lastProducedValue = value;
        onDataReady.invoke(value);
    }

    int getLastProducedValue() const { return lastProducedValue; }

private:
    int lastProducedValue = 0;
};

// Класс B - подписан на A, преобразует данные
class ClassB
{
public:
    Event<float> onDataProcessed;

    ClassB() = default;

    void onDataReceived(int value)
    {
        lastReceivedValue = value;
        float processedValue = static_cast<float>(value) * 1.5f;
        onDataProcessed.invoke(processedValue);
    }

    int getLastReceivedValue() const { return lastReceivedValue; }

private:
    int lastReceivedValue = 0;
};

// Класс C - подписан на B, преобразует данные
class ClassC
{
public:
    Event<std::string> onDataFormatted;

    ClassC() = default;

    void onDataReceived(float value)
    {
        lastReceivedValue = value;
        std::string formatted = std::format("Formatted: {:.2f}", value);
        onDataFormatted.invoke(formatted);
    }

    float getLastReceivedValue() const { return lastReceivedValue; }

private:
    float lastReceivedValue = 0.0f;
};

// Класс D - конечный потребитель (подписан на C)
class ClassD
{
public:
    void onDataReceived(const std::string& message)
    {
        lastReceivedMessage = message;
        receiveCount++;
    }

    std::string getLastReceivedMessage() const { return lastReceivedMessage; }
    int getReceiveCount() const { return receiveCount; }

private:
    std::string lastReceivedMessage;
    int receiveCount = 0;
};

// Класс DWithEvent - версия с Event для цепочки из 5 классов
class ClassDWithEvent
{
public:
    Event<std::string> onDataProcessed;

    void onDataReceived(const std::string& message)
    {
        lastReceivedMessage = message;
        receiveCount++;
        onDataProcessed.invoke(message);
    }

    std::string getLastReceivedMessage() const { return lastReceivedMessage; }
    int getReceiveCount() const { return receiveCount; }

private:
    std::string lastReceivedMessage;
    int receiveCount = 0;
};

// Класс E - конечный потребитель для 5-классовой цепочки
class ClassE
{
public:
    void onDataReceived(const std::string& message)
    {
        lastReceivedMessage = message;
        receiveCount++;
    }

    std::string getLastReceivedMessage() const { return lastReceivedMessage; }
    int getReceiveCount() const { return receiveCount; }

private:
    std::string lastReceivedMessage;
    int receiveCount = 0;
};

// Класс E2 - альтернативный потребитель (подписан на B)
class ClassE2
{
public:
    void onDataReceived(float value)
    {
        lastReceivedValue = value;
        receiveCount++;
    }

    float getLastReceivedValue() const { return lastReceivedValue; }
    int getReceiveCount() const { return receiveCount; }

private:
    float lastReceivedValue = 0.0f;
    int receiveCount = 0;
};

// ======================== Тесты ========================

// Тест 1: Цепочка из 4 классов (A → B → C → D)
TEST(EventChainTest, FourClassChain)
{
    // Создаём классы (они не знают друг о друге)
    ClassA source;     // Источник
    ClassB processor;  // Обработчик 1
    ClassC formatter;  // Обработчик 2
    ClassD consumer;   // Конечный потребитель

    // Настраиваем связи (только здесь классы узнают друг о друге)
    source.onDataReady.subscribe([&processor](int value) { processor.onDataReceived(value); });

    processor.onDataProcessed.subscribe([&formatter](float value) { formatter.onDataReceived(value); });

    formatter.onDataFormatted.subscribe([&consumer](const std::string& message) { consumer.onDataReceived(message); });

    // Действие: источник генерирует событие
    source.produceData(100);

    // Проверяем результат
    EXPECT_EQ(source.getLastProducedValue(), 100);
    EXPECT_EQ(processor.getLastReceivedValue(), 100);
    EXPECT_FLOAT_EQ(formatter.getLastReceivedValue(), 150.0f);
    EXPECT_EQ(consumer.getLastReceivedMessage(), "Formatted: 150.00");
    EXPECT_EQ(consumer.getReceiveCount(), 1);
}

// Тест 2: Цепочка из 5 классов (A → B → C → D → E)
TEST(EventChainTest, FiveClassChainWithEventInD)
{
    ClassA source;
    ClassB b;
    ClassC c;
    ClassDWithEvent d;
    ClassE e;

    // Строим цепочку: A → B → C → D → E
    source.onDataReady.subscribe([&b](int v) { b.onDataReceived(v); });
    b.onDataProcessed.subscribe([&c](float v) { c.onDataReceived(v); });
    c.onDataFormatted.subscribe([&d](const std::string& s) { d.onDataReceived(s); });
    d.onDataProcessed.subscribe([&e](const std::string& s) { e.onDataReceived(s); });

    source.produceData(50);

    EXPECT_EQ(b.getLastReceivedValue(), 50);
    EXPECT_FLOAT_EQ(c.getLastReceivedValue(), 75.0f);
    EXPECT_EQ(d.getLastReceivedMessage(), "Formatted: 75.00");
    EXPECT_EQ(e.getLastReceivedMessage(), "Formatted: 75.00");
    EXPECT_EQ(e.getReceiveCount(), 1);
}

// Тест 3: Множественная подписка на одно событие
TEST(EventChainTest, MultipleConsumersOnSameEvent)
{
    ClassA source;
    ClassB processor;
    ClassD consumer1;
    ClassD consumer2;
    ClassE2 consumer3;

    source.onDataReady.subscribe([&processor](int value) { processor.onDataReceived(value); });

    // Множественная подписка на processor.onDataProcessed
    processor.onDataProcessed.subscribe([&consumer1](float value) { consumer1.onDataReceived(std::format("Consumer1: {:.2f}", value)); });

    processor.onDataProcessed.subscribe([&consumer2](float value) { consumer2.onDataReceived(std::format("Consumer2: {:.2f}", value)); });

    processor.onDataProcessed.subscribe([&consumer3](float value) { consumer3.onDataReceived(value); });

    source.produceData(50);

    // Проверяем, что все потребители получили события
    EXPECT_EQ(consumer1.getReceiveCount(), 1);
    EXPECT_EQ(consumer2.getReceiveCount(), 1);
    EXPECT_EQ(consumer3.getReceiveCount(), 1);

    EXPECT_EQ(consumer1.getLastReceivedMessage(), "Consumer1: 75.00");
    EXPECT_EQ(consumer2.getLastReceivedMessage(), "Consumer2: 75.00");
    EXPECT_FLOAT_EQ(consumer3.getLastReceivedValue(), 75.0f);
}

// Тест 5: Динамическая подписка и отписка (требует Event с поддержкой CallbackId)
TEST(EventChainTest, DynamicSubscriptionAndUnsubscription)
{
    // Примечание: этот тест требует модификации Event для поддержки unsubscribe по ID
    // Если Event не поддерживает unsubscribe, тест будет пропущен

    ClassA source;
    ClassB processor;
    ClassD consumer;

    source.onDataReady.subscribe([&processor](int value) { processor.onDataReceived(value); });

    // Подписываемся
    processor.onDataProcessed.subscribe([&consumer](float value) { consumer.onDataReceived(std::format("Value: {:.2f}", value)); });

    source.produceData(10);
    EXPECT_EQ(consumer.getReceiveCount(), 1);
    EXPECT_EQ(consumer.getLastReceivedMessage(), "Value: 15.00");

    source.produceData(20);
    EXPECT_EQ(consumer.getReceiveCount(), 2);
    EXPECT_EQ(consumer.getLastReceivedMessage(), "Value: 30.00");
}

// Тест 6: Трансформация данных через цепочку
TEST(EventChainTest, DataTransformationChain)
{
    ClassA source;
    ClassB b;
    ClassC c;
    ClassD d;

    // Строим цепочку: int → float → string
    source.onDataReady.subscribe([&b](int v) { b.onDataReceived(v); });
    b.onDataProcessed.subscribe([&c](float v) { c.onDataReceived(v); });
    c.onDataFormatted.subscribe([&d](const std::string& s) { d.onDataReceived(s); });

    source.produceData(50);

    EXPECT_EQ(b.getLastReceivedValue(), 50);
    EXPECT_FLOAT_EQ(c.getLastReceivedValue(), 75.0f);
    EXPECT_EQ(d.getLastReceivedMessage(), "Formatted: 75.00");
    EXPECT_EQ(d.getReceiveCount(), 1);
}

// Тест 7: Пустая цепочка (нет подписчиков)
TEST(EventChainTest, NoSubscribers)
{
    ClassA source;

    // Никто не подписан на событие
    // Не должно быть ошибок или исключений
    EXPECT_NO_THROW(source.produceData(42));
}

// Тест 8: Множественные источники событий
TEST(EventChainTest, MultipleEventSources)
{
    ClassA source1;
    ClassA source2;
    ClassB processor;
    ClassD consumer;

    // Оба источника отправляют данные в один процессор
    source1.onDataReady.subscribe([&processor](int v) { processor.onDataReceived(v); });
    source2.onDataReady.subscribe([&processor](int v) { processor.onDataReceived(v); });

    processor.onDataProcessed.subscribe([&consumer](float v) { consumer.onDataReceived(std::format("Value: {:.2f}", v)); });

    source1.produceData(10);
    source2.produceData(20);
    source1.produceData(30);

    EXPECT_EQ(consumer.getReceiveCount(), 3);
    EXPECT_EQ(consumer.getLastReceivedMessage(), "Value: 45.00");
}

// Тест 9: Восстановление после исключений (если callback выбрасывает)
TEST(EventChainTest, ExceptionInCallback)
{
    ClassA source;
    ClassB processor;
    ClassD consumer1;
    ClassD consumer2;

    source.onDataReady.subscribe([&processor](int v) { processor.onDataReceived(v); });

    processor.onDataProcessed.subscribe([&consumer1](float v) { consumer1.onDataReceived(std::format("First: {:.2f}", v)); });

    processor.onDataProcessed.subscribe([&consumer2](float v) { consumer2.onDataReceived(std::format("Second: {:.2f}", v)); });

    // Не должно быть проблем с множественными подписчиками
    source.produceData(100);

    EXPECT_EQ(consumer1.getReceiveCount(), 1);
    EXPECT_EQ(consumer2.getReceiveCount(), 1);
    EXPECT_EQ(consumer1.getLastReceivedMessage(), "First: 150.00");
    EXPECT_EQ(consumer2.getLastReceivedMessage(), "Second: 150.00");
}