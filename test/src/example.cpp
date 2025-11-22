#include <iostream>
#include <asserter/src/test.hpp>
#include <glfw_wrapper/src/glfw_wrapper.hpp>

int main()
{
    using namespace glfw_wrapper;
    bool init_success = init_glfw(std::cout);

    ASSERT(init_success);

    auto window = Window::make_window(500, 300, false, true, "test");

    ASSERT(window.impl());
    ASSERT(window.is_valid());

    bool touch_event_emitted = false;
    auto touch_listener = window.touch_emitter()->hook([&touch_event_emitted](auto current, auto previous) {
        touch_event_emitted = true;
    });

    ASSERT(!touch_event_emitted);
    window.emit_touch_event();
    ASSERT(touch_event_emitted);

    auto key_listener = window.key_emitter()->hook([](auto current, auto previous) { 
        std::cout << "key: " << current << " <- " << previous << std::endl;
    });

    auto scroll_listener = window.scroll_emitter()->hook([](auto current, auto previous) {
        std::cout << "scroll: " << current.x << " " << current.y << " <- " << previous.x << " " << previous.y << std::endl;
    });

    auto resize_listener = window.resize_emitter()->hook([](auto current, auto previous) {
        std::cout << "resize: " << current.x << " " << current.y << " <- " << previous.x << " " << previous.y << std::endl;
    });

#if 0
    while (!window.should_close())
    {
        poll_events();
    }
#endif

    terminate();

    return 0;
}
