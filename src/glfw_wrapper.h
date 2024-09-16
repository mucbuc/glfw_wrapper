#pragma once

#include <iostream>
#include <vector>

struct GLFWwindow;
namespace glfw_wrapper
{
    struct vec2f
    {
        float_t x;
        float_t y;
    };

    struct mouse_button_state
    {
        bool m_left_button_down; // mac mouse doesn't have right button, better for touch screen anyway
        vec2f m_mouse;
    };

    struct keyboard_state
    {
        std::vector<std::string> m_pressed;
    };

	struct Window {
        Window() = default;
        static Window make_window(uint w, uint h, bool passThrough, bool opaque, std::string title);

        bool should_close();
        void poll_events();
        void get_mouse_pos(double & x, double & y);
        
        mouse_button_state current_mouse_button_state();
        mouse_button_state previous_mouse_button_state();
        
        keyboard_state curent_keyboard_state();
        keyboard_state previous_keyboard_state();
        
        void get_window_pos(int & left, int & top);
        void set_window_pos(int left, int top);
        void get_window_size(int & width, int & height);

        GLFWwindow* impl() const;
        ~Window();
        void setFrameVisible(bool value);
    
        void update_previous_mouse_pos();
        void update_keyboard_state();
    
    private:
        struct Pimpl;
        std::shared_ptr<Pimpl> m_pimpl;
        Window(std::shared_ptr<Pimpl>);
    };
    
    bool init_glfw(std::ostream &);
    void poll_events();
    void terminate();
    
}
