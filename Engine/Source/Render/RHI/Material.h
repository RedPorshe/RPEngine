// Render/RHI/Material.h
#pragma once
#include <string>
#include <vector>
#include "Math/MathTypes.h"

namespace RPE
{

class Material
{
public:
    Material() = default;
    virtual ~Material() = default;

    void setName(const std::string& name) { m_name = name; }
    std::string getName() const { return m_name; }
    bool loadMaterial(const std::string& path);
    // Параметры материала
    void setDiffuseColor(const FVector& color) { m_diffuseColor = color; }
    void setSpecularColor(const FVector& color) { m_specularColor = color; }
    void setEmissiveColor(const FVector& color) { m_emissiveColor = color; }
    void setShininess(float shininess) { m_shininess = shininess; }
    void setOpacity(float opacity) { m_opacity = opacity; }

    FVector getDiffuseColor() const { return m_diffuseColor; }
    FVector getSpecularColor() const { return m_specularColor; }
    FVector getEmissiveColor() const { return m_emissiveColor; }
    float getShininess() const { return m_shininess; }
    float getOpacity() const { return m_opacity; }

    void setDiffuseTexture(const std::string& path) { m_diffuseTexture = path; }
    void setNormalTexture(const std::string& path) { m_normalTexture = path; }
    void setSpecularTexture(const std::string& path) { m_specularTexture = path; }

    std::string getDiffuseTexture() const { return m_diffuseTexture; }
    std::string getNormalTexture() const { return m_normalTexture; }
    std::string getSpecularTexture() const { return m_specularTexture; }
    bool isLoaded() const;

private:
    bool bIsLoaded = false;
    std::string m_name;
    FVector m_diffuseColor = FVector(1.0f);
    FVector m_specularColor = FVector(1.0f);
    FVector m_emissiveColor = FVector(0.0f);
    float m_shininess = 32.0f;
    float m_opacity = 1.0f;

    std::string m_diffuseTexture;
    std::string m_normalTexture;
    std::string m_specularTexture;
};

}  // namespace RPE