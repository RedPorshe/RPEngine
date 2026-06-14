#pragma once
#include "Core/SystemObject.h"

namespace RPE
{
class RGameInstance : public CObject
{
    CHUDDO_DECLARE_CLASS(RGameInstance, CObject);

public:
    RGameInstance(const std::string& inDisplayName = "RGameInstance", CObject* inOwner = nullptr);
    virtual ~RGameInstance();

    virtual void Init();
    virtual void tick(float deltaTime);
    virtual void onDestroy();

protected:
};

}  // namespace RPE
namespace RPE
{
REGISTER_CLASS_FACTORY(RGameInstance);
}
