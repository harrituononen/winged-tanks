#ifndef ENTITIES_CONSUMABLE_RESOURCE_HPP
#define ENTITIES_CONSUMABLE_RESOURCE_HPP

namespace entities {

template<typename T>
class ConsumableResource final
{
public:
    ConsumableResource  (T const max_val);
    ConsumableResource  (T const start_val, T const max_val);
    ~ConsumableResource () = default;

    ConsumableResource            (ConsumableResource const&) = delete;
    ConsumableResource& operator= (ConsumableResource const&) = delete;

    T max_value;
    T value;
};

} // namespace entities

#include "consumable_resource.tpp"

#endif // ENTITIES_CONSUMABLE_RESOURCE_HPP
