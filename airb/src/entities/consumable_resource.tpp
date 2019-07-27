namespace entities {

template<typename T>
ConsumableResource<T>::ConsumableResource(T const max_val)
    : ConsumableResource(max_val, max_val)
{ }

template<typename T>
ConsumableResource<T>::ConsumableResource(T const start_val, T const max_val)
    : max_value(max_val)
    , value(start_val)
{ }

} // namespace entities
