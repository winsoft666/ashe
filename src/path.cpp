#include "ashe/path.h"
#include "ashe/string_encode.h"
#include "ashe/string_util.h"

namespace ashe {
Path::Path(const char* p) {
    if (p) {
#ifdef ASHE_WIN
        s_ = a2w(p);
#else
        s_ = p;
#endif
    }
}

Path::Path(const std::string& s) {
#ifdef ASHE_WIN
    s_ = a2w(s);
#else
    s_ = s;
#endif
}

Path::Path(std::string&& s) {
#ifdef ASHE_WIN
    s_ = a2w(s);
#else
    s_ = s;
#endif
}

Path::Path(const wchar_t* p) {
    if (p) {
#ifdef ASHE_WIN
        s_ = p;
#else
        s_ = w2u(p);
#endif
    }
}

Path::Path(const std::wstring& s) {
#ifdef ASHE_WIN
    s_ = s;
#else
    s_ = w2u(s);
#endif
}

Path::Path(std::wstring&& s) {
#ifdef ASHE_WIN
    s_ = s;
#else
    s_ = w2u(s);
#endif
}

Path& Path::operator=(std::wstring&& s) noexcept {
#ifdef ASHE_WIN
    s_ = s;
#else
    s_ = w2u(s);
#endif
    return *this;
}

Path& Path::operator=(std::string&& s) noexcept {
#ifdef ASHE_WIN
    s_ = a2w(s);
#else
    s_ = s;
#endif
    return *this;
}

Path& Path::operator=(const std::wstring& s) {
#ifdef ASHE_WIN
    s_ = s;
#else
    s_ = w2u(s);
#endif
    return *this;
}

Path& Path::operator=(const std::string& s) {
#ifdef ASHE_WIN
    s_ = a2w(s);
#else
    s_ = s;
#endif
    return *this;
}

Path::operator std::string() const {
#ifdef ASHE_WIN
    return w2a(s_);
#else
    return s_;
#endif
}

Path::operator std::wstring() const {
#ifdef ASHE_WIN
    return s_;
#else
    return u2w(s_);
#endif
}

Path Path::operator+(const Path& that) {
#ifdef ASHE_WIN
    std::wstring s1 = static_cast<std::wstring>(*this);
    std::wstring s2 = static_cast<std::wstring>(that);
    std::wstring sep = static_cast<std::wstring>(Path::Separator());
#else
    std::string s1 = static_cast<std::string>(*this);
    std::string s2 = static_cast<std::string>(that);
    std::string sep = static_cast<std::string>(Path::Separator());
#endif
    if (!StrIsEndsWith(s1, sep))
        s1 += sep;
    return Path(s1 + s2);
}

Path& Path::operator+=(const Path& that) {
#ifdef ASHE_WIN
    std::wstring s2 = static_cast<std::wstring>(that);
    std::wstring sep = static_cast<std::wstring>(Path::Separator());
#else
    std::string s2 = static_cast<std::string>(that);
    std::string sep = static_cast<std::string>(Path::Separator());
#endif
    if (!StrIsEndsWith(s_, sep)) {
        s_ += sep;
    }
    s_ += s2;
    return *this;
}

bool Path::empty() const {
    return s_.empty();
}

Path Path::Separator() {
#ifdef ASHE_WIN
    return Path("\\");
#else
    return Path("/");
#endif
}

}  // namespace ashe