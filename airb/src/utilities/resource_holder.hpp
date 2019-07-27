#ifndef UTILITIES_RESOURCE_HOLDER_HPP
#define UTILITIES_RESOURCE_HOLDER_HPP

#include <map>
#include <memory>

namespace utilities {

template<typename T, typename Id>
class ResourceHolder final
{
private:
    std::map<Id, std::unique_ptr<T>> m_resources;

public:
     ResourceHolder();
    ~ResourceHolder();

    ResourceHolder            (ResourceHolder const&) = delete;
    ResourceHolder& operator= (ResourceHolder const&) = delete;

    void load    (Id const id, std::unique_ptr<T> res_ptr);
    void destroy (Id const id);
    T&   get     (Id const id);
};

} // namespace utilities

#include "resource_holder.tpp"

#endif // UTILITIES_RESOURCE_HOLDER_HPP
