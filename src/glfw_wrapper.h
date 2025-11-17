#pragma once

#include <iostream>
#include <vector>
#include <string>

struct GLFWwindow;
namespace glfw_wrapper {
struct vec2f {
    float x;
    float y;
};

struct mouse_button_state {
    bool m_left_button_down; // mac mouse doesn't have right button, better for touch screen anyway
    vec2f m_mouse;
};

struct keyboard_state {
    std::vector<std::string> m_pressed;
};

struct Window {
    Window() = default;
    static Window make_window(unsigned w, unsigned h, bool passThrough, bool opaque, std::string title);

    bool should_close();
    void get_mouse_pos(double& x, double& y);

    void set_mouse_move(std::function<void(double, double)>);
    void set_window_resize(std::function<void(double, double)>);
    void set_window_scroll(std::function<void(double, double)>);
    void set_key_press(std::function<void(std::string)>); 

    mouse_button_state current_mouse_button_state();
    mouse_button_state previous_mouse_button_state();

    keyboard_state curent_keyboard_state();
    keyboard_state previous_keyboard_state();

    void get_window_pos(int& left, int& top);
    void set_window_pos(int left, int top);
    void get_window_size(int& width, int& height);
    void get_framebuffer_size(int& width, int& height);

    GLFWwindow* impl() const;
    ~Window();
    void setFrameVisible(bool value);

    void update_previous_mouse_pos();
    void update_keyboard_state();

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
