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

    bool mouse_event_invoked = false;
    auto l = window.mouse_events()->hook([& mouse_event_invoked](auto current, auto previous){
    
        mouse_event_invoked = true; 

        // if (!current.m_left_button_down && previous.m_left_button_down)
        // {
        //     std::cout << "touch up" << std::endl;
        // }
        
        // std::cout << current.m_position.x << " " << current.m_position.y << std::endl;
    }); 

    ASSERT(!mouse_event_invoked);
    window.invoke_mouse_event();
    ASSERT(mouse_event_invoked);

    //while (!window.should_close()) {
    //    poll_events();
    //}

    return 0;
}
