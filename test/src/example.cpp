#include <iostream>
#include <lib/asserter/src/test.hpp>
#include <lib/glfw_wrapper/src/glfw_wrapper.h>

int main()
{
    using namespace glfw_wrapper;
    bool init_success = init_glfw(std::cout);

    ASSERT(init_success);

    auto window = Window::make_window(500, 300, false, true, "test");

    ASSERT(window.impl());

    bool touch_event_emitted = false;
    auto touch_listener = window.touch_emitter()->hook([& touch_event_emitted](auto current, auto previous){
        touch_event_emitted = true; 
    }); 

    ASSERT(!touch_event_emitted);
    window.emit_touch_event();
    ASSERT(touch_event_emitted);

    auto key_listener = window.key_emitter()->hook([window](auto current, auto previous) mutable {
        if (current == "a")
        {
            int left;
            int top; 
            window.get_window_pos(left, top);
            window.set_window_pos(left + 10, top - 10);
        }

        if (current == "Escape") {
            window.close(); 
        }
    });

    // while (!window.should_close())
    // {
    //     poll_events();
    // }

    return 0;
}
