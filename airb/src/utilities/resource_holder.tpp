#include <cassert>
#include <utility>

#include "debug.hpp"

namespace utilities {

template<typename T, typename Id>
ResourceHolder<T, Id>::ResourceHolder()
    : m_resources ()
{
    Debug::log("Resource holder created.");
}

template<typename T, typename Id>
ResourceHolder<T, Id>::~ResourceHolder()
{
    Debug::log("Resource holder destroyed.");
}

template<typename T, typename Id>
void ResourceHolder<T, Id>::load(Id const id, std::unique_ptr<T> res_ptr)
{
    m_resources.emplace(std::make_pair(id, std::move(res_ptr)));
}

template<typename T, typename Id>
void ResourceHolder<T, Id>::destroy(Id const id)
{
    m_resources.erase(id);
}

template<typename T, typename Id>
T& ResourceHolder<T, Id>::get(Id const id)
{
    auto itr = m_resources.find(id);
    assert(itr != m_resources.end());
    return *itr->second;
}

} // namespace utilities
