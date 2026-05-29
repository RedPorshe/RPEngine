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
    void processInput(const InputEvent& event);
};
}  // namespace RPE