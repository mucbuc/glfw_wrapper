#pragma once

#include <lib/circuit/src/index.hpp>

#include "emitter_impl.hpp"
#include "interface.hpp"

namespace om636 {
namespace control {

    template <typename T>
    struct default_policy {
        static void pushed_event(const T&) {}
        static bool locked_mutex(const T&) { return true; }
        static void unlocked_mutex(const T&) {}
    };

    template <class T, template <typename> class P, typename... U>
    class QuemitterImpl
        : public Quemitter<T, U...> {
        typedef Quemitter<T, U...> base_type;

    public:
        using typename base_type::callback_type;
        using typename base_type::event_type;
        using typename base_type::listener_type;

        listener_type on(event_type, callback_type) override;
        listener_type once(event_type, callback_type) override;

        void interupt(event_type, U...) override;
        void emit(event_type, U...) override;

    private:
        typedef P<QuemitterImpl<T, P, U...>> fbp;
        typedef std::function<void()> function_type;
        typedef om636::circuit::CircuitQueue<function_type> queue_type;
        typedef std::mutex mutex_type;

        void push_event(function_type&&);

        EmitterImpl<T, U...> m_reactor;
        queue_type m_queue;
        mutex_type m_mutex;
    };

} //control
} // om636

#include "quemitter_impl.hxx"
