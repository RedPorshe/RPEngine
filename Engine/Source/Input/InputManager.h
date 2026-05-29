#pragma once
#include "Core/Utility.h"

namespace RPE
{
struct InputEvent;

class InputManager : public NonCopyable
{
public:
    InputManager();
    virtual ~InputManager();
    void proccessInput(const InputEvent& event);
};
}  // namespace RPE