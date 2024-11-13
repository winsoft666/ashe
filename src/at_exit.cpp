#include "ashe/at_exit.h"
#include <stddef.h>
#include <ostream>

namespace ashe {

// Keep a stack of registered AtExitManagers.  We always operate on the most
// recent, and we should never have more than one outside of testing (for a
// statically linked version of this library).
// Testing may use the shadow version of the constructor, and if we are building a dynamic library we may
// end up with multiple AtExitManagers on the same process.
// We don't protect this for thread-safe access, since it will only be modified in testing.
static AtExitManager* g_top_manager = nullptr;

AtExitManager::AtExitManager() {
    g_top_manager = this;
}

AtExitManager::~AtExitManager() {
    if (!g_top_manager) {
        return;
    }
    ProcessCallbacksNow();
    g_top_manager = nullptr;
}

void AtExitManager::RegisterCallback(AtExitCallbackType func) {
    if (!g_top_manager) {
        return;
    }
    std::lock_guard<std::mutex> lg(g_top_manager->m_);
    g_top_manager->stack_.push(func);
}

void AtExitManager::ProcessCallbacksNow() {
    if (!g_top_manager) {
        return;
    }

    std::lock_guard<std::mutex> lg(g_top_manager->m_);

    while (!g_top_manager->stack_.empty()) {
        AtExitCallbackType cb = g_top_manager->stack_.top();
        if (cb) {
            cb();
        }
        g_top_manager->stack_.pop();
    }
}
}  // namespace ashe
