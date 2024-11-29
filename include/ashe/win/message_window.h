#ifndef ASHE_WIN_MESSAGE_WINDOW_H_
#define ASHE_WIN_MESSAGE_WINDOW_H_

#include "ashe/macros.h"
#include <functional>
#include <Windows.h>
#include "ashe/thread_checker.h"

namespace ashe {
namespace win {
class MessageWindow {
   public:
    MessageWindow() = default;
    ~MessageWindow();

    // Implement this callback to handle messages received by the message window.
    // If the callback returns |false|, the first four parameters are passed to
    // DefWindowProc(). Otherwise, |*result| is returned by the window procedure.
    using MessageCallback = std::function<bool(UINT message,
                                               WPARAM wparam,
                                               LPARAM lparam,
                                               LRESULT& result)>;

    // Creates a message-only window. The incoming messages will be passed by
    // |message_callback|. |message_callback| must outlive |this|.
    bool create(MessageCallback message_callback);

    HWND hwnd() const;

   private:
    static bool registerWindowClass(HINSTANCE instance);
    static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

    MessageCallback message_callback_;
    HWND hwnd_ = nullptr;

    THREAD_CHECKER(thread_checker_);

    ASHE_DISALLOW_COPY_AND_ASSIGN(MessageWindow);
};
}  // namespace win
}  // namespace ashe

#endif  // BASE_WIN_MESSAGE_WINDOW_H
