namespace om636 {
namespace control {

    /////////////////////////////////////////////////////////////////////////////////////
    template <typename T, template <typename> class P, typename... U>
    auto QuemitterImpl<T, P, U...>::on(event_type e, callback_type c) -> listener_type
    {
        return m_reactor.on(e, c);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    template <typename T, template <typename> class P, typename... U>
    auto QuemitterImpl<T, P, U...>::once(event_type e, callback_type c) -> listener_type
    {
        return m_reactor.once(e, c);
    }

    /////////////////////////////////////////////////////////////////////////////////////
    template <typename T, template <typename> class P, typename... U>
    void QuemitterImpl<T, P, U...>::interupt(event_type e, U... arg)
    {
        m_reactor.interupt(e, arg...);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T, template <typename> class P, typename... U>
    void QuemitterImpl<T, P, U...>::emit(event_type e, U... v)
    {
        function_type p([e, v..., this]() {
            interupt(e, v...);
        });

        push_event(std::move(p));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    template <class T, template <typename> class P, typename... U>
    void QuemitterImpl<T, P, U...>::push_event(function_type && f)
    {
        m_queue.push(std::move(f));
        fbp::pushed_event(*this);

	function_type current;
        std::unique_lock<mutex_type> lock(m_mutex, std::defer_lock);
        while (lock.try_lock()
            && fbp::locked_mutex(*this)
            && m_queue.check_pop(current)) {
            current();

            lock.unlock();
            fbp::unlocked_mutex(*this);
        }
    }

} //control
} // om636
