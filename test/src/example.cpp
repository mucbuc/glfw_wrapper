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
    auto l = window.touch_emitter()->hook([& touch_event_emitted](auto current, auto previous){
    
        touch_event_emitted = true; 

        // if (!current.m_is_down && previous.m_is_down)
        // {
        //     std::cout << "touch up" << std::endl;
        // }
        
        // std::cout << current.m_position.x << " " << current.m_position.y << std::endl;
    }); 

    ASSERT(!touch_event_emitted);
    window.emit_touch_event();
    ASSERT(touch_event_emitted);

    //while (!window.should_close()) {
    //    poll_events();
    //}

    return 0;
}
