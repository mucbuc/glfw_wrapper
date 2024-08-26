#include "glfw_wrapper.h"

#include <GLFW/glfw3.h>
#include <algorithm>

using namespace std;
        
namespace {
    static bool glfw_initialized = false;
}

namespace glfw_wrapper
{

    void terminate()
    {
        glfwTerminate();
    }

	bool init_glfw(ostream & sout)
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
    
    struct Window::Pimpl
    {
        
        template<class T>
        Pimpl(T w) : m_window(move(w))
        {
            glfwSetWindowUserPointer(m_window.get(), this);
            
            glfwSetMouseButtonCallback(m_window.get(), & mouse_button_callback);
            glfwSetKeyCallback(m_window.get(), & key_callback);
        }
        
        bool should_close()
        {
            return glfwWindowShouldClose(impl());
        }
        
        void setFrameVisible(bool value)
        {
            glfwWindowHint(GLFW_DECORATED, value ? GLFW_TRUE : GLFW_FALSE);
        }
        
        void get_mouse_pos(double & x, double & y)
        {
            glfwGetCursorPos(impl(), & x, & y);
            
            lock_guard<mutex> guard(m_mouse_button);
            m_current_mouse_button.m_mouse = vec2d<float_t> { float_t(x), float_t(y) };
        }

        void get_window_pos(int & left, int & top)
        {
            glfwGetWindowPos(impl(), & left, & top);
        }
        
        void get_window_size(int & width, int & height)
        {
            glfwGetWindowSize(impl(), & width, & height);
        }
        
        void set_window_pos(int left, int top)
        {
            glfwSetWindowPos(impl(), left, top);
        }
        
        GLFWwindow * impl() const
        {
            return m_window.get();
        }
        
        mouse_button_state current_mouse_button_state()
        {
            lock_guard<mutex> guard(m_mouse_button);
            return m_current_mouse_button;
        }
        
        mouse_button_state previous_mouse_button_state()
        {
            lock_guard<mutex> guard(m_mouse_button);
            return m_previous_mouse_button;
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

        void update_previous_mouse_pos()
        {
            lock_guard<mutex> guard(m_mouse_button);
            m_previous_mouse_button = m_current_mouse_button;
        }
    
        void update_keyboard_state()
        {
            lock_guard<mutex> guard(m_keyboard_state);
            m_previous_keyboard_state = m_current_keyboard_state;
        }
    
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
        {
            auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl *>(glfwGetWindowUserPointer(window));
            
            //ASSERT(pimpl);
        
            lock_guard<mutex> guard(pimpl->m_mouse_button);
        
            if (action == GLFW_PRESS) {
//                if (button == GLFW_MOUSE_BUTTON_RIGHT)
//                {
//                    pimpl->m_current_mouse_button.m_right_button_down = true;
//                }
//                else
                if (button == GLFW_MOUSE_BUTTON_LEFT)
                {
                    pimpl->m_current_mouse_button.m_left_button_down = true;
                }
            }
            else if (action == GLFW_RELEASE) {
//                if (button == GLFW_MOUSE_BUTTON_RIGHT)
//                {
//                    pimpl->m_current_mouse_button.m_right_button_down = false;
//                }
//                else
                if (button == GLFW_MOUSE_BUTTON_LEFT)
                {
                    pimpl->m_current_mouse_button.m_left_button_down = false;
                }
            }
        }
        
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl *>(glfwGetWindowUserPointer(window));
        
            if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
            {
                lock_guard<mutex> guard(pimpl->m_keyboard_state);
            
                const auto target = string(1, 'a' + key - GLFW_KEY_A);
        
                if (action == GLFW_PRESS)
                {
                    pimpl->m_current_keyboard_state.m_pressed.push_back(target);
                }
                else if (action == GLFW_RELEASE)
                {
                    const auto i = find(pimpl->m_current_keyboard_state.m_pressed.begin(), pimpl->m_current_keyboard_state.m_pressed.end(), target);
                    if (i != pimpl->m_current_keyboard_state.m_pressed.end())
                    {
                        pimpl->m_current_keyboard_state.m_pressed.erase(i);
                    }
                }
            }
            else if (key == GLFW_KEY_ESCAPE)
            {
                lock_guard<mutex> guard(pimpl->m_keyboard_state);
                if (action == GLFW_PRESS)
                {
                    pimpl->m_current_keyboard_state.m_pressed.push_back("ESCAPE");
                }
                else if (action == GLFW_RELEASE)
                {
                    const auto i = find(pimpl->m_current_keyboard_state.m_pressed.begin(), pimpl->m_current_keyboard_state.m_pressed.end(), "ESCAPE");
                    if (i != pimpl->m_current_keyboard_state.m_pressed.end())
                    {
                        pimpl->m_current_keyboard_state.m_pressed.erase(i);
                    }
                }
            }
        }
    
        unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_window;
        
        mouse_button_state m_current_mouse_button = { false, {0, 0} };
        mouse_button_state m_previous_mouse_button = { false, {0, 0} };
        
        keyboard_state m_current_keyboard_state;
        keyboard_state m_previous_keyboard_state;
    
        mutex m_mouse_button;
        mutex m_keyboard_state;
    };
    
    Window Window::make_window(uint w, uint h, bool passThrough, bool opaque)
    {
        if (!glfw_initialized) {
            return Window { nullptr };
        }
    
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        
        if (!opaque) {
            glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        }
        else {
            glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        }
        glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, passThrough ? GLFW_TRUE : GLFW_FALSE);
        
        //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    
        const auto name = string("Morph");//_" + to_string(w) + "x" + to_string(h);
        
        GLFWwindow * raw_ptr = glfwCreateWindow(w, h, name.c_str(), nullptr, nullptr);
        return make_shared<Window::Pimpl>(unique_ptr<GLFWwindow, decltype(& glfwDestroyWindow)>(raw_ptr, glfwDestroyWindow));
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
    
    void Window::setFrameVisible(bool value)
    {
        m_pimpl->setFrameVisible(value);
    }
    
    void Window::get_mouse_pos(double & x, double & y)
    {
        m_pimpl->get_mouse_pos(x, y);
    }

    void Window::get_window_pos(int & left, int & top)
    {
        m_pimpl->get_window_pos(left, top);
    }
    
    void Window::get_window_size(int & width, int & height)
    {
        m_pimpl->get_window_size(width, height);
    }
    
    void Window::set_window_pos(int left, int top)
    {
        m_pimpl->set_window_pos(left, top);
    }
    
    GLFWwindow * Window::impl() const
    {
        return m_pimpl->m_window.get();
    }
    
    mouse_button_state Window::current_mouse_button_state()
    {
        return m_pimpl->current_mouse_button_state();
    }
    
    mouse_button_state Window::previous_mouse_button_state()
    {
        return m_pimpl->previous_mouse_button_state();
    }
        
    keyboard_state Window::curent_keyboard_state()
    {
        return m_pimpl->current_keyboard_state();
    }
    
    keyboard_state Window::previous_keyboard_state()
    {
        return m_pimpl->previous_keyboard_state();
    }

    void Window::update_previous_mouse_pos()
    {
        m_pimpl->update_previous_mouse_pos();
    }
    
    void Window::update_keyboard_state()
    {
        m_pimpl->update_keyboard_state();
    }
 
} // glfw_wrapper
