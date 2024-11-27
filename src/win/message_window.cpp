#include "ashe/win/message_window.h"
#include <atomic>
#include "ashe/check_failure.h"

namespace ashe {
namespace win {
namespace {
constexpr wchar_t kWindowClassName[] = L"ashe_win_MessageWindowClass";
constexpr wchar_t kWindowName[] = L"ashe_win_MessageWindow";
static std::atomic_bool _class_registered = false;
}  // namespace

MessageWindow::~MessageWindow() {
    DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);

    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

bool MessageWindow::create(MessageCallback message_callback) {
    DCHECK_CALLED_ON_VALID_THREAD(thread_checker_);
    ASHE_CHECK_FAILURE(!hwnd_, nullptr);

    message_callback_ = std::move(message_callback);

    HINSTANCE instance = nullptr;

    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<wchar_t*>(&windowProc),
                            &instance)) {
        ASHE_CHECK_FAILURE(false, "GetModuleHandleExW failed");
        return false;
    }

    if (!registerWindowClass(instance)) {
        ASHE_CHECK_FAILURE(false, "Unable to create window class");

        return false;
    }

    hwnd_ = CreateWindowW(kWindowClassName,
                          kWindowName,
                          0, 0, 0, 0, 0,
                          nullptr,
                          nullptr,
                          instance,
                          this);
    if (!hwnd_) {
        ASHE_CHECK_FAILURE(false, "CreateWindowW failed");
        return false;
    }

    ShowWindow(hwnd_, SW_HIDE);
    return true;
}

HWND MessageWindow::hwnd() const {
    return hwnd_;
}

LRESULT CALLBACK MessageWindow::windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    MessageWindow* self = reinterpret_cast<MessageWindow*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));

    switch (msg) {
            // Set up the self before handling WM_CREATE.
        case WM_CREATE: {
            LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>(lParam);
            self = reinterpret_cast<MessageWindow*>(cs->lpCreateParams);

            // Make |hwnd| available to the message handler. At this point the
            // control hasn't returned from CreateWindow() yet.
            self->hwnd_ = window;

            // Store pointer to the self to the window's user data.
            SetLastError(ERROR_SUCCESS);
            LONG_PTR result =
                SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
            ASHE_CHECK_FAILURE((result != 0 || GetLastError() == ERROR_SUCCESS), nullptr);
        } break;

        // Clear the pointer to stop calling the self once WM_DESTROY is
        // received.
        case WM_DESTROY: {
            SetLastError(ERROR_SUCCESS);
            LONG_PTR result = SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            ASHE_CHECK_FAILURE((result != 0 || GetLastError() == ERROR_SUCCESS), nullptr);
        } break;

        default:
            break;
    }

    // Handle the message.
    if (self) {
        LRESULT message_result;

        if (self->message_callback_(msg, wParam, lParam, message_result))
            return message_result;
    }

    return DefWindowProcW(window, msg, wParam, lParam);
}

bool MessageWindow::registerWindowClass(HINSTANCE instance) {
    if (_class_registered) {
        return true;
    }

    WNDCLASSEXW window_class;
    memset(&window_class, 0, sizeof(window_class));

    window_class.cbSize = sizeof(window_class);
    window_class.lpszClassName = kWindowClassName;
    window_class.hInstance = instance;
    window_class.lpfnWndProc = windowProc;

    if (!RegisterClassExW(&window_class)) {
        ASHE_CHECK_FAILURE(false, "RegisterClassExW failed");
        return false;
    }

    _class_registered = true;
    return true;
}
}  // namespace win
}  // namespace ashe
