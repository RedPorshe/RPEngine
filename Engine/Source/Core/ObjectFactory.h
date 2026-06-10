#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <type_traits>
#include <mutex>
#include <unordered_set>
#include <stack>

#define REGISTER_CLASS_FACTORY(ClassName)                                  \
    static bool Register##ClassName()                                      \
    {                                                                      \
        static bool bRegistered = false;                                   \
        if (!bRegistered)                                                  \
        {                                                                  \
            RPE::CObjectFactory::GetInstance().registerClass<ClassName>(); \
            bRegistered = true;                                            \
        }                                                                  \
        return bRegistered;                                                \
    }                                                                      \
    static bool ClassName##_Registered = Register##ClassName();

namespace RPE
{

class CObject;

class CObjectFactory
{
public:
    using CreatorFunc = std::function<CObject*(const std::string&, CObject*)>;

    static CObjectFactory& GetInstance()
    {
        static CObjectFactory instance;
        return instance;
    }

    bool IsDerivedFrom(const std::string& ClassName, const std::string& BaseClassName) const;

    CObjectFactory(const CObjectFactory&) = delete;
    CObjectFactory& operator=(const CObjectFactory&) = delete;

    template <typename T>
    void registerClass()
    {
        static_assert(std::is_base_of<CObject, T>::value, "CObjectFactory: Class must derive from CObject");

        std::string className = T::StaticClassName();

        static std::mutex registrationMutex;
        std::lock_guard<std::mutex> lock(registrationMutex);

        static std::unordered_set<std::string> registeredClasses;

        if (registeredClasses.find(className) != registeredClasses.end()) return;
        if (ClassCreators.find(className) != ClassCreators.end())
        {
            registeredClasses.insert(className);
            return;
        }

        CreatorFunc creator = [](const std::string& displayName, CObject* owner) -> CObject* { return new T(displayName, owner); };

        ClassCreators[className] = creator;

        std::string baseClassName = T::StaticBaseClassName();
        ClassHierarchy[className] = (className == "CObject") ? "" : baseClassName;

        registeredClasses.insert(className);

#ifdef _DEBUG
        std::cout << "[FACTORY] Registered class '" << className << "' (base: '" << (baseClassName.empty() ? "None" : baseClassName)
                  << "')\n";
#endif
    }

    template <typename T>
    void RegisterClass(const std::string& className)
    {
        static_assert(std::is_base_of<CObject, T>::value, "CObjectFactory: Class must derive from CObject");

        static std::mutex registrationMutex;
        std::lock_guard<std::mutex> lock(registrationMutex);

        static std::unordered_set<std::string> registeredClasses;

        if (registeredClasses.find(className) != registeredClasses.end()) return;
        if (ClassCreators.find(className) != ClassCreators.end())
        {
            registeredClasses.insert(className);
            return;
        }

        ClassCreators[className] = [](const std::string& displayName, CObject* owner) -> CObject* { return new T(displayName, owner); };

        ClassHierarchy[className] = (className == "CObject") ? "" : T::StaticBaseClassName();

        registeredClasses.insert(className);

#ifdef _DEBUG
        std::cout << "[FACTORY] Registered class '" << className << "'\n";
#endif
    }

    CObject* Create(const std::string& className, CObject* owner = nullptr, const std::string& displayName = "Object");

    bool IsClassRegistered(const std::string& className) const { return ClassCreators.find(className) != ClassCreators.end(); }

    std::vector<std::string> GetRegisteredClasses() const
    {
        std::vector<std::string> classes;
        for (const auto& [name, _] : ClassCreators)
            classes.push_back(name);
        return classes;
    }

    std::string GetClassHierarchy() const
    {
        std::stringstream ss;
        ss << "Class Hierarchy:\n";
        for (const auto& [className, baseClass] : ClassHierarchy)
        {
            ss << "  " << className << " -> " << (baseClass.empty() ? "None" : baseClass) << "\n";
        }
        return ss.str();
    }

private:
    CObjectFactory() = default;
    ~CObjectFactory() = default;

    std::unordered_map<std::string, CreatorFunc> ClassCreators;
    std::unordered_map<std::string, std::string> ClassHierarchy;

    std::string FindParentClass(const std::string& className) const
    {
        auto it = ClassHierarchy.find(className);
        return (it != ClassHierarchy.end()) ? it->second : "";
    }
};

}  // namespace RPE

#define OBJECT_FACTORY RPE::CObjectFactory::GetInstance()