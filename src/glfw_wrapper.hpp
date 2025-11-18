#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <lib/dynamo/src/interface.hpp>

struct GLFWwindow;
namespace glfw_wrapper {
struct vec2f {
    float x;
    float y;
};

struct vec2i {
    int x;
    int y;
};

struct touch_state {
    bool is_down;
    bool right_is_down;
    vec2f position;
};

struct Window {
    Window() = default;
    static Window make_window(unsigned w, unsigned h, bool passThrough, bool opaque, std::string title);

    bool should_close();
    void close();

    using touch_emitter_type = std::shared_ptr<om636::control::Batch<touch_state, touch_state>>;
    touch_emitter_type touch_emitter();

    // this is to retrieve current touch position before touch events have been emitted
    void emit_touch_event();

    using key_emitter_type = std::shared_ptr<om636::control::Batch<std::string, std::string>>;
    key_emitter_type key_emitter();
    bool is_valid_key(const std::string&);
    std::vector<std::string> get_all_keys();

    using resize_emitter_type = std::shared_ptr<om636::control::Batch<vec2i, vec2i>>;
    resize_emitter_type resize_emitter();

    using scroll_emitter_type = std::shared_ptr<om636::control::Batch<vec2f, vec2f>>;
    scroll_emitter_type scroll_emitter();

    vec2i get_window_pos();
    void set_window_pos(vec2i);
    vec2i get_window_size();
    vec2i get_framebuffer_size();

    GLFWwindow* impl() const;
    ~Window();

    bool is_valid() const;

private:
    struct Pimpl;
    std::shared_ptr<Pimpl> m_pimpl;
    Window(std::shared_ptr<Pimpl>);
};

bool init_glfw(std::ostream&);
void poll_events();
void terminate();

}
