#include "pch.h"
#include "Implementation.h"

namespace Graphics4
{
    List<ImplementationInfo> implementations;
    Map<String, ImplementationInfo*> implementationMap;

    ImplementationInfo* Graphics4::RegisterImplementation(const ImplementationInfo& info)
    {
        auto& ptr = *implementations.AddBack(info);
        implementationMap.Insert(info.name, &ptr);
        return &ptr;
    }
    ImplementationInfo* Graphics4::GetImplementationInfo(String name)
    {
        return implementationMap.Find(name)->value;
    }
}