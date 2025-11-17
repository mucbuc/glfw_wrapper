#pragma once

#include <iostream>
#include <vector>
#include <string>

#include <lib/ohm/src/interface.hpp>

struct GLFWwindow;
namespace glfw_wrapper {
struct vec2f {
    float x;
    float y;
};

struct touch_state {
    bool m_is_down;
    vec2f m_position;
};

struct Window {
    Window() = default;
    static Window make_window(unsigned w, unsigned h, bool passThrough, bool opaque, std::string title);

    bool should_close();

    using touch_emitter_type = std::shared_ptr<om636::control::Batch<touch_state, touch_state>>;
    touch_emitter_type touch_emitter();

    // this is to retrieve current touch position before touch events have been emitted
    void emit_touch_event();

    using key_emitter_type = std::shared_ptr<om636::control::Batch<std::string, std::string>>;
    key_emitter_type key_emitter();

    void set_window_resize(std::function<void(double, double)>);
    void set_window_scroll(std::function<void(double, double)>);

    void get_window_pos(int& left, int& top);
    void set_window_pos(int left, int top);
    void get_window_size(int& width, int& height);
    void get_framebuffer_size(int& width, int& height);

    GLFWwindow* impl() const;
    ~Window();
    void setFrameVisible(bool value);

    operator bool() const;

private:
    struct Pimpl;
    std::shared_ptr<Pimpl> m_pimpl;
    Window(std::shared_ptr<Pimpl>);
};

bool init_glfw(std::ostream&);
void poll_events();
void terminate();

}
