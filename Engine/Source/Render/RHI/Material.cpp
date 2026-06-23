#include "Material.h"

#include "Log/Log.h"

DEFINE_LOG_CATEGORY_STATIC(MATERIALlog);

bool RPE::Material::loadMaterial(const std::string& path)
{
    RP_LOG(MATERIALlog, Warning, "Loading materials not implemented yet");
    bIsLoaded = false;  // MaterialManager::Get()->LoadMaterial(path); TODO:
    return bIsLoaded;
}

bool RPE::Material::isLoaded() const
{
    return bIsLoaded;
}
