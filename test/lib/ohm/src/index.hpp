#pragma once
#include <memory>

#include "emitter_impl.hpp"
#include "quemitter_impl.hpp"

namespace om636 {
namespace control {

    template <typename T, typename... U>
    std::shared_ptr<Emitter<T, U...>> make_emitter()
    {
        return std::make_shared<EmitterImpl<T, U...>>();
    }

    template <typename T, typename... U>
    std::shared_ptr<Emitter<T, U...>> make_quemitter()
    {
        return std::make_shared<QuemitterImpl<T, default_policy, U...>>();
    }

} // control
} // om636
