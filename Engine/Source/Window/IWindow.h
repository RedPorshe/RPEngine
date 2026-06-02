#pragma once

#include <string>
#include <functional>

namespace RPE
{

struct WindowId
{
    unsigned int value{0};
    constexpr explicit WindowId(unsigned int id) : value(id) {}

    constexpr WindowId operator++(int)
    {
        WindowId temp = *this;
        ++value;
        return temp;
    }

    constexpr auto operator<=>(const WindowId&) const = default;
};

struct WindowSettings
{
    std::string title{};
    int width{800};
    int height{600};
    int x{50};
    int y{50};
};

class IWindow
{
public:
    virtual ~IWindow() = default;

    virtual void setTitle(const std::string& title) = 0;
    virtual bool isValid() const = 0;
    virtual bool shouldClose() const = 0;
    virtual void* getWindowHandle() const = 0;
};

}  // namespace RPE

namespace std
{
template <>
struct hash<RPE::WindowId>
{
    size_t operator()(const RPE::WindowId& id) const noexcept  //
    {
        return std::hash<unsigned int>{}(id.value);
    }
};
}  // namespace std