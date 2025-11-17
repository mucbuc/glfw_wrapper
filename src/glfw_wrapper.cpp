#include "glfw_wrapper.h"

#include <lib/asserter/src/asserter.hpp>
#include <lib/dynamo/src/impl/batch.hpp>


#include <GLFW/glfw3.h>
#include <algorithm>

using namespace std;

namespace {
static bool glfw_initialized = false;
}

namespace glfw_wrapper {

void terminate()
{
    glfwTerminate();
}

bool init_glfw(ostream& sout)
{
    if (!glfw_initialized && !glfwInit()) {
        sout << "glfwInit failed!" << endl;
        return false;
    }
    glfw_initialized = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    return true;
}

void poll_events()
{
    glfwPollEvents();
}

struct Window::Pimpl {

    template <class T>
    Pimpl(T w)
        : m_window(std::move(w))
    {
        glfwSetWindowUserPointer(m_window.get(), this);

        double x;
        double y; 
        glfwGetCursorPos(m_window.get(), &x, &y);
        m_current_touch.m_position = vec2f { float(x), float(y) };
        m_previous_touch.m_position = m_current_touch.m_position;

        glfwSetMouseButtonCallback(m_window.get(), &mouse_button_callback);
        glfwSetCursorPosCallback(m_window.get(), &handle_mouse_move);

        glfwSetKeyCallback(m_window.get(), &key_callback);
        glfwSetCharCallback(m_window.get(), &char_callback);

        glfwSetWindowSizeCallback(m_window.get(), &window_size_callback); 
    
        m_mouse_batch = om636::control::make_queue<touch_state, touch_state>();
    }

    bool should_close()
    {
        return glfwWindowShouldClose(impl());
    }

    Window::mouse_event_batch mouse_events()
    {
        return m_mouse_batch;
    }

    void setFrameVisible(bool value)
    {
        glfwWindowHint(GLFW_DECORATED, value ? GLFW_TRUE : GLFW_FALSE);
    }

    void invoke_mouse_event()
    {
        m_mouse_batch->invoke(m_current_touch, m_previous_touch);
    }

    void set_window_resize(std::function<void(double, double)> cb)
    {
        m_on_window_resize = cb;
    }

    void set_window_scroll(std::function<void(double, double)> cb)
    {
        m_on_scroll = cb;
    }    

    void set_key_press(std::function<void(string)> cb)
    {
        m_on_key_press = cb;
    }        

    void get_window_pos(int& left, int& top)
    {
        glfwGetWindowPos(impl(), &left, &top);
    }

    void get_window_size(int& width, int& height)
    {
        glfwGetWindowSize(impl(), &width, &height);
    }

    void get_framebuffer_size(int & width, int & height)
    {
        glfwGetFramebufferSize(impl(), &width, &height);
    }

    void set_window_pos(int left, int top)
    {
        glfwSetWindowPos(impl(), left, top);
    }

    GLFWwindow* impl() const
    {
        return m_window.get();
    }

    keyboard_state current_keyboard_state()
    {
        lock_guard<mutex> guard(m_keyboard_state);
        return m_current_keyboard_state;
    }

    keyboard_state previous_keyboard_state()
    {
        lock_guard<mutex> guard(m_keyboard_state);
        return m_previous_keyboard_state;
    }

    void update_keyboard_state()
    {
        lock_guard<mutex> guard(m_keyboard_state);
        m_previous_keyboard_state = m_current_keyboard_state;
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));

        ASSERT(pimpl);

        if (action == GLFW_PRESS) {
            //                if (button == GLFW_MOUSE_BUTTON_RIGHT)
            //                {
            //                    pimpl->m_current_touch.m_right_button_down = true;
            //                }
            //                else
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                pimpl->m_current_touch.m_is_down = true;
            }
        } else if (action == GLFW_RELEASE) {
            //                if (button == GLFW_MOUSE_BUTTON_RIGHT)
            //                {
            //                    pimpl->m_current_touch.m_right_button_down = false;
            //                }
            //                else
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                pimpl->m_current_touch.m_is_down = false;
            }
        }

        pimpl->m_mouse_batch->invoke(pimpl->m_current_touch, pimpl->m_previous_touch);
        pimpl->m_previous_touch = pimpl->m_current_touch;
    }

    static void char_callback(GLFWwindow* window, unsigned int codepoint)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));


        std::cout << "char_callback: " << codepoint << std::endl;
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
// #define GLFW_KEY_LEFT_SHIFT         340
// #define GLFW_KEY_RIGHT_SHIFT        344

        std::cout << "key pressed: " << key << " " << scancode << std::endl;

        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS) {
            if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
                pimpl->m_on_key_press(string(1, 'a' + key - GLFW_KEY_A));
            }
            else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
            {
                pimpl->m_on_key_press(string(1, '0' + key - GLFW_KEY_0));
            }
            else {
                switch (key) {
                    case GLFW_KEY_ENTER:
                        pimpl->m_on_key_press("Enter"); 
                        break;
                
                    case GLFW_KEY_ESCAPE:
                        pimpl->m_on_key_press("Escape");
                        break;
                };
            }
        }
    }

    static void handle_mouse_move(GLFWwindow* window, double xpos, double ypos)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));

        pimpl->m_current_touch.m_position.x = xpos;
        pimpl->m_current_touch.m_position.y = ypos;
        
        pimpl->m_mouse_batch->invoke(pimpl->m_current_touch, pimpl->m_previous_touch);
        pimpl->m_previous_touch = pimpl->m_current_touch;
    }

    static void window_size_callback(GLFWwindow* window, int width, int height)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));

        ASSERT(pimpl->m_on_window_resize);
        pimpl->m_on_window_resize(width, height);
    }


    unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_window;

    touch_state m_current_touch = { false, { 0, 0 } };
    touch_state m_previous_touch = { false, { 0, 0 } };

    keyboard_state m_current_keyboard_state;
    keyboard_state m_previous_keyboard_state;

    mutex m_keyboard_state;

    std::function<void(double, double)> m_on_window_resize;
    std::function<void(double, double)> m_on_scroll;
    std::function<void(string)> m_on_key_press;
    Window::mouse_event_batch m_mouse_batch;
};

Window Window::make_window(unsigned w, unsigned h, bool passThrough, bool opaque, std::string title)
{
    if (!glfw_initialized) {
        return Window { nullptr };
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (!opaque) {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    } else {
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    }
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, passThrough ? GLFW_TRUE : GLFW_FALSE);

    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    const auto name = string(title);
    GLFWwindow* raw_ptr = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
    return make_shared<Window::Pimpl>(unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(raw_ptr, glfwDestroyWindow));
}

Window::Window(shared_ptr<Pimpl> p)
    : m_pimpl(p)
{
}

Window::~Window() = default;

bool Window::should_close()
{
    return glfwWindowShouldClose(impl());
}

auto Window::mouse_events() -> mouse_event_batch
{
    return m_pimpl->mouse_events();
}

void Window::setFrameVisible(bool value)
{
    m_pimpl->setFrameVisible(value);
}

void Window::invoke_mouse_event()
{
    m_pimpl->invoke_mouse_event();
}

void Window::set_window_resize(std::function<void(double, double)> cb)
{
    m_pimpl->set_window_resize(cb);
}    

void Window::set_window_scroll(std::function<void(double, double)> cb)
{
    m_pimpl->set_window_scroll(cb);
}    

void Window::set_key_press(std::function<void(std::string)> cb)
{
    m_pimpl->set_key_press(cb);
}     

void Window::get_window_pos(int& left, int& top)
{
    m_pimpl->get_window_pos(left, top);
}

void Window::get_window_size(int& width, int& height)
{
    m_pimpl->get_window_size(width, height);
}

void Window::get_framebuffer_size(int& width, int& height)
{
    m_pimpl->get_framebuffer_size(width, height);
}

void Window::set_window_pos(int left, int top)
{
    m_pimpl->set_window_pos(left, top);
}

GLFWwindow* Window::impl() const
{
    return m_pimpl->m_window.get();
}

keyboard_state Window::curent_keyboard_state()
{
    return m_pimpl->current_keyboard_state();
}

keyboard_state Window::previous_keyboard_state()
{
    return m_pimpl->previous_keyboard_state();
}

void Window::update_keyboard_state()
{
    m_pimpl->update_keyboard_state();
}

Window::operator bool() const
{
    return bool(m_pimpl); 
}

} // glfw_wrapper
