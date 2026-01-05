#include "glfw_wrapper.hpp"

#include <asserter/src/asserter.hpp>
#include <dynamo/src/impl/batch.hpp>

#include <GLFW/glfw3.h>
#include <algorithm>
#include <map>

using namespace std;

namespace {
static bool glfw_initialized = false;

static std::map<unsigned, std::string> get_key_map();

static std::vector<std::string> get_all_keys(const std::map<unsigned, std::string>& key_map)
{
    std::vector<std::string> keys;
    keys.reserve(key_map.size());
    for (const auto& entry : key_map) {
        keys.push_back(entry.second);
    }

    return keys;
}

} // private

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
        , m_current_touch { false, false, { 0, 0 } }
        , m_current_key {}
        , m_current_size {}
        , m_current_scroll { 0, 0 }
        , m_touch_emitter(om636::control::make_queue<touch_state, touch_state>())
        , m_key_emitter(om636::control::make_queue<std::string, std::string>())
        , m_resize_emitter(om636::control::make_queue<vec2i, vec2i>())
        , m_scroll_emitter(om636::control::make_queue<vec2f, vec2f>())
        , m_closed(false)
        , m_key_map(get_key_map())
        , m_all_keys(::get_all_keys(m_key_map))
    {
        glfwSetWindowUserPointer(m_window.get(), this);

        double x;
        double y;
        glfwGetCursorPos(m_window.get(), &x, &y);
        m_current_touch.position = vec2f { float(x), float(y) };

        m_current_size = get_window_size();

        glfwSetMouseButtonCallback(m_window.get(), &mouse_button_callback);
        glfwSetCursorPosCallback(m_window.get(), &handle_mouse_move);

        glfwSetKeyCallback(m_window.get(), &key_callback);

        // glfwSetCharCallback(m_window.get(), &char_callback);

        glfwSetWindowSizeCallback(m_window.get(), &window_size_callback);
        glfwSetScrollCallback(m_window.get(), &window_scroll_callback);
    }

    Window::touch_emitter_type touch_emitter()
    {
        return m_touch_emitter;
    }

    void emit_touch_event()
    {
        m_touch_emitter->invoke(m_current_touch, m_current_touch);
    }

    void update_current_touch(touch_state touch)
    {
        m_touch_emitter->invoke(touch, m_current_touch);
        m_current_touch = touch;
    }

    void update_scroll(vec2f offset)
    {
        m_scroll_emitter->invoke(offset, m_current_scroll);
        m_current_scroll = offset;
    }

    void update_size(vec2i size)
    {
        m_resize_emitter->invoke(size, m_current_size);
        m_current_size = size;
    }

    Window::key_emitter_type key_emitter()
    {
        return m_key_emitter;
    }

    Window::resize_emitter_type resize_emitter()
    {
        return m_resize_emitter;
    }

    Window::scroll_emitter_type scroll_emitter()
    {
        return m_scroll_emitter;
    }

    void update_current_key(std::string key)
    {
        m_key_emitter->invoke(key, m_current_key);
        m_current_key = key;
    }

    vec2i get_window_pos()
    {
        vec2i result;
        glfwGetWindowPos(impl(), &result.x, &result.y);
        return result;
    }

    vec2i get_window_size()
    {
        vec2i result;
        glfwGetWindowSize(impl(), &result.x, &result.y);
        return result;
    }

    vec2i get_framebuffer_size()
    {
        vec2i result;
        glfwGetFramebufferSize(impl(), &result.x, &result.y);
        return result;
    }

    void set_window_pos(vec2i size)
    {
        glfwSetWindowPos(impl(), size.x, size.y);
    }

    GLFWwindow* impl() const
    {
        return m_window.get();
    }

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));

        ASSERT(pimpl);

        if (action == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                touch_state touch = pimpl->m_current_touch;
                touch.right_is_down = true;
                pimpl->update_current_touch(touch);
            } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
                touch_state touch = pimpl->m_current_touch;
                touch.is_down = true;
                pimpl->update_current_touch(touch);
            }
        } else if (action == GLFW_RELEASE) {
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                touch_state touch = pimpl->m_current_touch;
                touch.right_is_down = false;
                pimpl->update_current_touch(touch);
            } else if (button == GLFW_MOUSE_BUTTON_LEFT) {
                touch_state touch = pimpl->m_current_touch;
                touch.is_down = false;
                pimpl->update_current_touch(touch);
            }
        }
    }

#if 0 
    static void char_callback(GLFWwindow* window, unsigned int codepoint)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));

        std::cout << "char_callback: " << codepoint << std::endl;
    }
#endif

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            const auto pos = pimpl->m_key_map.find(key);
            if (pos != pimpl->m_key_map.end()) {
                pimpl->update_current_key(pos->second);
            }
        } else if (action == GLFW_RELEASE) {
            pimpl->update_current_key("");
        }
    }

    static void handle_mouse_move(GLFWwindow* window, double xpos, double ypos)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));

        touch_state touch = pimpl->m_current_touch;
        touch.position = vec2f { float(xpos), float(ypos) };
        pimpl->update_current_touch(touch);
    }

    static void window_size_callback(GLFWwindow* window, int width, int height)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));
        pimpl->update_size(vec2i { width, height });
    }

    static void window_scroll_callback(GLFWwindow* window, double x, double y)
    {
        auto pimpl = reinterpret_cast<glfw_wrapper::Window::Pimpl*>(glfwGetWindowUserPointer(window));
        pimpl->update_scroll(vec2f { float(x), float(y) });
    }

    unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_window;

    touch_state m_current_touch;
    std::string m_current_key;
    vec2i m_current_size;
    vec2f m_current_scroll;
    Window::touch_emitter_type m_touch_emitter;
    Window::key_emitter_type m_key_emitter;
    Window::resize_emitter_type m_resize_emitter;
    Window::scroll_emitter_type m_scroll_emitter;
    std::atomic<bool> m_closed;
    const std::map<unsigned, std::string> m_key_map;
    const std::vector<std::string> m_all_keys;
};

Window Window::make_window(unsigned w, unsigned h, bool passThrough, bool opaque, std::string title)
{
    if (!glfw_initialized) {
        return Window { nullptr };
    }

#ifndef __EMSCRIPTEN__
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

#else 
    // prevent from context creation
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif

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

void Window::close()
{
    glfwSetWindowShouldClose(impl(), true);
}

auto Window::touch_emitter() -> touch_emitter_type
{
    return m_pimpl->touch_emitter();
}

void Window::emit_touch_event()
{
    m_pimpl->emit_touch_event();
}

auto Window::key_emitter() -> key_emitter_type
{
    return m_pimpl->key_emitter();
}

auto Window::resize_emitter() -> resize_emitter_type
{
    return m_pimpl->resize_emitter();
}

auto Window::scroll_emitter() -> scroll_emitter_type
{
    return m_pimpl->scroll_emitter();
}

bool Window::is_valid_key(const std::string& key) const
{
    const auto& p = std::find(m_pimpl->m_all_keys.begin(), m_pimpl->m_all_keys.end(), key);
    return p != m_pimpl->m_all_keys.end();
}

std::vector<std::string> Window::get_all_keys() const
{
    return m_pimpl->m_all_keys;
}

vec2i Window::get_window_pos()
{
    return m_pimpl->get_window_pos();
}

vec2i Window::get_window_size()
{
    return m_pimpl->get_window_size();
}

vec2i Window::get_framebuffer_size()
{
    return m_pimpl->get_framebuffer_size();
}

void Window::set_window_pos(vec2i pos)
{
    m_pimpl->set_window_pos(pos);
}

GLFWwindow* Window::impl() const
{
    ASSERT(!m_pimpl->m_closed);
    return m_pimpl->m_window.get();
}

bool Window::is_valid() const
{
    return bool(m_pimpl);
}

} // glfw_wrapper

namespace {

std::map<unsigned, std::string> get_key_map()
{
    return {
        { GLFW_KEY_SPACE, "SPACE" },
        { GLFW_KEY_APOSTROPHE, "APOSTROPHE" },
        { GLFW_KEY_COMMA, "COMMA" },
        { GLFW_KEY_MINUS, "MINUS" },
        { GLFW_KEY_PERIOD, "PERIOD" },
        { GLFW_KEY_SLASH, "SLASH" },
        { GLFW_KEY_0, "0" },
        { GLFW_KEY_1, "1" },
        { GLFW_KEY_2, "2" },
        { GLFW_KEY_3, "3" },
        { GLFW_KEY_4, "4" },
        { GLFW_KEY_5, "5" },
        { GLFW_KEY_6, "6" },
        { GLFW_KEY_7, "7" },
        { GLFW_KEY_8, "8" },
        { GLFW_KEY_9, "9" },
        { GLFW_KEY_SEMICOLON, "SEMICOLON" },
        { GLFW_KEY_EQUAL, "EQUAL" },
        { GLFW_KEY_A, "A" },
        { GLFW_KEY_B, "B" },
        { GLFW_KEY_C, "C" },
        { GLFW_KEY_D, "D" },
        { GLFW_KEY_E, "E" },
        { GLFW_KEY_F, "F" },
        { GLFW_KEY_G, "G" },
        { GLFW_KEY_H, "H" },
        { GLFW_KEY_I, "I" },
        { GLFW_KEY_J, "J" },
        { GLFW_KEY_K, "K" },
        { GLFW_KEY_L, "L" },
        { GLFW_KEY_M, "M" },
        { GLFW_KEY_N, "N" },
        { GLFW_KEY_O, "O" },
        { GLFW_KEY_P, "P" },
        { GLFW_KEY_Q, "Q" },
        { GLFW_KEY_R, "R" },
        { GLFW_KEY_S, "S" },
        { GLFW_KEY_T, "T" },
        { GLFW_KEY_U, "U" },
        { GLFW_KEY_V, "V" },
        { GLFW_KEY_W, "W" },
        { GLFW_KEY_X, "X" },
        { GLFW_KEY_Y, "Y" },
        { GLFW_KEY_Z, "Z" },
        { GLFW_KEY_LEFT_BRACKET, "LEFT_BRACKET" },
        { GLFW_KEY_BACKSLASH, "BACKSLASH" },
        { GLFW_KEY_RIGHT_BRACKET, "RIGHT_BRACKET" },
        { GLFW_KEY_GRAVE_ACCENT, "GRAVE_ACCENT" },
        { GLFW_KEY_WORLD_1, "WORLD_1" },
        { GLFW_KEY_WORLD_2, "WORLD_2" },

        /* Function keys */
        { GLFW_KEY_ESCAPE, "ESCAPE" },
        { GLFW_KEY_ENTER, "ENTER" },
        { GLFW_KEY_TAB, "TAB" },
        { GLFW_KEY_BACKSPACE, "BACKSPACE" },
        { GLFW_KEY_INSERT, "INSERT" },
        { GLFW_KEY_DELETE, "DELETE" },
        { GLFW_KEY_RIGHT, "RIGHT" },
        { GLFW_KEY_LEFT, "LEFT" },
        { GLFW_KEY_DOWN, "DOWN" },
        { GLFW_KEY_UP, "UP" },
        { GLFW_KEY_PAGE_UP, "PAGE_UP" },
        { GLFW_KEY_PAGE_DOWN, "PAGE_DOWN" },
        { GLFW_KEY_HOME, "HOME" },
        { GLFW_KEY_END, "END" },
        { GLFW_KEY_CAPS_LOCK, "CAPS_LOCK" },
        { GLFW_KEY_SCROLL_LOCK, "SCROLL_LOCK" },
        { GLFW_KEY_NUM_LOCK, "NUM_LOCK" },
        { GLFW_KEY_PRINT_SCREEN, "PRINT_SCREEN" },
        { GLFW_KEY_PAUSE, "PAUSE" },
        { GLFW_KEY_F1, "F1" },
        { GLFW_KEY_F2, "F2" },
        { GLFW_KEY_F3, "F3" },
        { GLFW_KEY_F4, "F4" },
        { GLFW_KEY_F5, "F5" },
        { GLFW_KEY_F6, "F6" },
        { GLFW_KEY_F7, "F7" },
        { GLFW_KEY_F8, "F8" },
        { GLFW_KEY_F9, "F9" },
        { GLFW_KEY_F10, "F10" },
        { GLFW_KEY_F11, "F11" },
        { GLFW_KEY_F12, "F12" },
        { GLFW_KEY_F13, "F13" },
        { GLFW_KEY_F14, "F14" },
        { GLFW_KEY_F15, "F15" },
        { GLFW_KEY_F16, "F16" },
        { GLFW_KEY_F17, "F17" },
        { GLFW_KEY_F18, "F18" },
        { GLFW_KEY_F19, "F19" },
        { GLFW_KEY_F20, "F20" },
        { GLFW_KEY_F21, "F21" },
        { GLFW_KEY_F22, "F22" },
        { GLFW_KEY_F23, "F23" },
        { GLFW_KEY_F24, "F24" },
        { GLFW_KEY_F25, "F25" },
        { GLFW_KEY_KP_0, "KP_0" },
        { GLFW_KEY_KP_1, "KP_1" },
        { GLFW_KEY_KP_2, "KP_2" },
        { GLFW_KEY_KP_3, "KP_3" },
        { GLFW_KEY_KP_4, "KP_4" },
        { GLFW_KEY_KP_5, "KP_5" },
        { GLFW_KEY_KP_6, "KP_6" },
        { GLFW_KEY_KP_7, "KP_7" },
        { GLFW_KEY_KP_8, "KP_8" },
        { GLFW_KEY_KP_9, "KP_9" },
        { GLFW_KEY_KP_DECIMAL, "KP_DECIMAL" },
        { GLFW_KEY_KP_DIVIDE, "KP_DIVIDE" },
        { GLFW_KEY_KP_MULTIPLY, "KP_MULTIPLY" },
        { GLFW_KEY_KP_SUBTRACT, "KP_SUBTRACT" },
        { GLFW_KEY_KP_ADD, "KP_ADD" },
        { GLFW_KEY_KP_ENTER, "KP_ENTER" },
        { GLFW_KEY_KP_EQUAL, "KP_EQUAL" },
        { GLFW_KEY_LEFT_SHIFT, "LEFT_SHIFT" },
        { GLFW_KEY_LEFT_CONTROL, "LEFT_CONTROL" },
        { GLFW_KEY_LEFT_ALT, "LEFT_ALT" },
        { GLFW_KEY_LEFT_SUPER, "LEFT_SUPER" },
        { GLFW_KEY_RIGHT_SHIFT, "RIGHT_SHIFT" },
        { GLFW_KEY_RIGHT_CONTROL, "RIGHT_CONTROL" },
        { GLFW_KEY_RIGHT_ALT, "RIGHT_ALT" },
        { GLFW_KEY_RIGHT_SUPER, "RIGHT_SUPER" },
        { GLFW_KEY_MENU, "MENU" },
    };
}

} // private
