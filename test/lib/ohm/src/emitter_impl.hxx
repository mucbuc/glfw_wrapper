namespace om636 {
namespace control {

    /////////////////////////////////////////////////////////////////////////////////////
    template <typename T, typename... U>
    auto EmitterImpl<T, U...>::on(event_type e, callback_type c) -> listener_type
    {
        return m_repeat[e].hook(c);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    template <typename T, typename... U>
    auto EmitterImpl<T, U...>::once(event_type e, callback_type c) -> listener_type
    {
        return m_repeat[e].hook_once(c);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    template <typename T, typename... U>
    void EmitterImpl<T, U...>::interupt(event_type e, U... arg)
    {
        m_repeat[e].invoke(arg...);
    }

} // control
} // om636
