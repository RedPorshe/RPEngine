#pragma once
#include <string>
#include <functional>

struct GLFWwindow;

namespace RPE
{
struct WindowId
{
    unsigned int value{0};
    constexpr explicit WindowId(unsigned int inId) : value(inId) {}
    constexpr WindowId operator++(int)
    {
        WindowId temp = *this;
        ++value;
        return temp;
    }
    constexpr WindowId operator--(int)
    {
        WindowId temp = *this;
        --value;
        return temp;
    }
    constexpr auto operator<=>(const WindowId&) const = default;
};

struct WindowSettings
{
    std::string title{};
    int width{800};
    int height{600};
    int posx{50};
    int posy{50};
};

class GLFWWindow final
{
public:
    GLFWWindow(const WindowSettings& sSettings);
    ~GLFWWindow();
    void setTitle(const std::string& title);
    bool isValid() const;
    bool shouldClose() const;
    void shutdown();

private:
    GLFWwindow* m_window{nullptr};
};
}  // namespace RPE

namespace std
{
template <>
struct hash<RPE::WindowId>
{
    size_t operator()(const RPE::WindowId& windowid) const noexcept { return std::hash<unsigned int>{}(windowid.value); }
};
}  // namespace std