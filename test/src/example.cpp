#include <iostream>
#include <lib/asserter/src/test.hpp>
#include <lib/glfw_wrapper/src/glfw_wrapper.h>

int main()
{
    using namespace glfw_wrapper;
    bool init_success = init_glfw(std::cout);

    if (init_success) 
    {
        auto window = Window::make_window(500, 300, false, true, "test");
        while (!window.should_close()) {
            poll_events();
        }
    }

    return 0;
}
