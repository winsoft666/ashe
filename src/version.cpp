#include "ashe/version.h"
#include <assert.h>
#include <algorithm>
#include <cwchar>
#include <cstdlib>

namespace ashe {
Version::Version(const std::string& s) {
    const std::string sc = StrTrim(s);
    const std::vector<std::string> v = StrSplit(sc, ".");
    const bool valid = !v.empty() &&
                       std::find_if(v.begin(),
                                    v.end(),
                                    [](const std::string& c) {
                                        return !IsDigit(c);
                                    }) == v.end();
    if (valid) {
        for (const auto& c : v) {
            verElems_.push_back(std::strtol(c.c_str(), nullptr, 10));
        }
    }
}

Version::Version(const std::wstring& s) {
    const std::wstring sc = StrTrim(s);
    const std::vector<std::wstring> v = StrSplit(sc, L".");
    const bool valid = !v.empty() &&
                       std::find_if(v.begin(),
                                    v.end(),
                                    [](const std::wstring& c) {
                                        return !IsDigit(c);
                                    }) == v.end();
    if (valid) {
        for (const auto& c : v) {
            verElems_.push_back(std::wcstol(c.c_str(), nullptr, 10));
        }
    }
}

Version::Version(const Version& that) noexcept {
    this->verElems_ = that.verElems_;
}

bool Version::isValid() const noexcept {
    return !verElems_.empty();
}

bool Version::isSameFormat(const Version& that) const noexcept {
    return (this->verElems_.size() == that.verElems_.size());
}

bool Version::operator==(const Version& that) noexcept {
    if (!isSameFormat(that))
        return false;

    bool result = true;
    for (size_t i = 0; i < verElems_.size(); i++) {
        if (verElems_[i] != that.verElems_[i]) {
            result = false;
            break;
        }
    }
    return result;
}

bool Version::operator!=(const Version& that) noexcept {
    return !(*this == that);
}

bool Version::operator>(const Version& that) {
    return compare(that) > 0;
}

bool Version::operator<(const Version& that) {
    return compare(that) < 0;
}

bool Version::operator<=(const Version& that) {
    return compare(that) <= 0;
}

bool Version::operator>=(const Version& that) {
    return compare(that) >= 0;
}

Version& Version::operator=(const Version& that) noexcept {
    this->verElems_ = that.verElems_;
    return *this;
}

int Version::compare(const Version& that) {
    assert(isSameFormat(that));

    for (size_t i = 0; i < verElems_.size(); i++) {
        if (verElems_[i] > that.verElems_[i])
            return 1;

        if (verElems_[i] < that.verElems_[i])
            return -1;
    }
    return 0;
}
}  // namespace ashe