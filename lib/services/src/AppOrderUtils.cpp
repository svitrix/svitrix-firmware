#include "AppOrderUtils.h"

namespace
{
    bool contains(const std::vector<String>& v, const String& name)
    {
        for (size_t i = 0; i < v.size(); i++)
        {
            if (v[i] == name)
                return true;
        }
        return false;
    }
}

std::vector<String> orderApps(const std::vector<String>& savedOrder,
                              const std::vector<String>& desired)
{
    std::vector<String> result;
    result.reserve(desired.size());

    // 1. Emit saved order first, keeping only names that are still available.
    for (size_t i = 0; i < savedOrder.size(); i++)
    {
        const String& name = savedOrder[i];
        if (contains(desired, name) && !contains(result, name))
        {
            result.push_back(name);
        }
    }

    // 2. Append any available apps not covered by the saved order.
    for (size_t i = 0; i < desired.size(); i++)
    {
        const String& name = desired[i];
        if (!contains(result, name))
        {
            result.push_back(name);
        }
    }

    return result;
}
