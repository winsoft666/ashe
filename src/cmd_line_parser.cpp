#include "ashe/config.h"
#include "ashe/cmd_line_parser.h"
#include "ashe/arch.h"
#include "ashe/string_util.h"
#include "ashe/string_encode.h"

namespace ashe {
namespace {
const wchar_t delims[] = L"-/";
const wchar_t quotes[] = L"\"";
const wchar_t value_sep[] = L" :=";  // don't forget space!!
}  // namespace

CmdLineParser::CmdLineParser(const std::wstring& cmdline) {
    if (cmdline.length() > 0) {
        cmdline_ = cmdline;
        parse();
    }
}

CmdLineParser::CmdLineParser(std::wstring&& cmdline) {
    if (cmdline.length() > 0) {
        cmdline_ = std::move(cmdline);
        parse();
    }
}

CmdLineParser::CmdLineParser(const CmdLineParser& that) {
    cmdline_ = that.cmdline_;
    value_map_ = that.value_map_;
}

CmdLineParser::CmdLineParser(CmdLineParser&& that) noexcept {
    cmdline_ = std::move(that.cmdline_);
    value_map_ = std::move(that.value_map_);
}

CmdLineParser::ITERPOS CmdLineParser::begin() const {
    return value_map_.begin();
}

CmdLineParser::ITERPOS CmdLineParser::end() const {
    return value_map_.end();
}

bool CmdLineParser::hasKey(const std::wstring& key) const {
    ITERPOS it = findKey(key);

    if (it == value_map_.end())
        return false;

    return true;
}

bool CmdLineParser::hasVal(const std::wstring& key) const {
    ITERPOS it = findKey(key);

    if (it == value_map_.end())
        return false;

    if (it->second.length() == 0)
        return false;

    return true;
}

std::wstring CmdLineParser::getVal(const std::wstring& key) const {
    ITERPOS it = findKey(key);

    if (it == value_map_.end())
        return std::wstring();

    return it->second;
}

size_t CmdLineParser::getKeyCount() const {
    return value_map_.size();
}

void CmdLineParser::parse() {
    value_map_.clear();

    std::wstring strW = cmdline_;
    const wchar_t* sCurrent = strW.c_str();

    for (;;) {
        if (sCurrent[0] == L'\0')
            break;

        const wchar_t* sArg = wcspbrk(sCurrent, delims);

        if (!sArg)
            break;

        sArg++;

        if (sArg[0] == L'\0')
            break;  // ends with delim

        const wchar_t* sVal = wcspbrk(sArg, value_sep);

        if (sVal == NULL) {
            std::wstring Key(sArg);
            Key = StrToLower(Key);
            value_map_.insert(CmdLineParser::ValsMap::value_type(Key, L""));
            break;
        }
        else if (sVal[0] == L' ' || wcslen(sVal) == 1) {
            // cmdline ends with /Key: or a key with no value
            std::wstring Key(sArg, (int)(sVal - sArg));

            if (Key.length() > 0) {
                Key = StrToLower(Key);
                value_map_.insert(CmdLineParser::ValsMap::value_type(Key, L""));
            }

            sCurrent = sVal + 1;
            continue;
        }
        else {
            // key has value
            std::wstring Key(sArg, (int)(sVal - sArg));
            Key = StrToLower(Key);

            sVal++;

            const wchar_t* sQuote = wcspbrk(sVal, quotes);
            const wchar_t* sEndQuote = NULL;

            if (sQuote == sVal) {
                // string with quotes (defined in quotes, e.g. '")
                sQuote = sVal + 1;
                sEndQuote = wcspbrk(sQuote, quotes);
            }
            else {
                sQuote = sVal;
                sEndQuote = wcschr(sQuote, L' ');
            }

            if (sEndQuote == NULL) {
                // no end quotes or terminating space, take the rest of the string to its end
                std::wstring csVal(sQuote);

                if (Key.length() > 0) {
                    value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
                }

                break;
            }
            else {
                // end quote
                if (Key.length() > 0) {
                    std::wstring csVal(sQuote, (int)(sEndQuote - sQuote));
                    value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
                }

                sCurrent = sEndQuote + 1;
                continue;
            }
        }
    }
}

CmdLineParser::ITERPOS CmdLineParser::findKey(const std::wstring& key) const {
    const std::wstring keyLower = StrToLower(key);
    return value_map_.find(keyLower);
}
}  // namespace ashe
