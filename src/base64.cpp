#include "ashe/config.h"
#include "ashe/base64.h"

namespace ashe {

std::string Base64::Encode(std::string const& s, bool url) {
    return _Encode(s, url);
}

std::string Base64::EncodeWithPEM(std::string const& s) {
    return _EncodeWithPEM(s);
}

std::string Base64::EncodeWithMIME(std::string const& s) {
    return _EncodeWithMIME(s);
}

static int getPosOfChar(const unsigned char chr) {
    //
    // Return the position of chr within base64_encode()
    //

    if (chr >= 'A' && chr <= 'Z')
        return chr - 'A';
    else if (chr >= 'a' && chr <= 'z')
        return chr - 'a' + ('Z' - 'A') + 1;
    else if (chr >= '0' && chr <= '9')
        return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    else if (chr == '+' || chr == '-')
        return 62;  // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
    else if (chr == '/' || chr == '_')
        return 63;  // Ditto for '/' and '_'
    else
        return -1;
}

template <typename String>
static bool _Decode(String encoded_string, bool remove_linebreaks, std::vector<uint8_t>& ret) {
    //
    // decode(бн) is templated so that it can be used with String = const std::string&
    // or std::string_view (requires at least C++17)
    //
    if (encoded_string.empty())
        return false;

    if (remove_linebreaks) {
        std::string copy(encoded_string);

        copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());

        ret = Base64::Decode(copy, false);
        return ret.size() > 0;
    }

    const size_t length_of_string = encoded_string.length();
    size_t pos = 0;

    //
    // The approximate length (bytes) of the decoded string might be one or
    // two bytes smaller, depending on the amount of trailing equal signs
    // in the encoded string. This approximation is needed to reserve
    // enough space in the string to be returned.
    //
    const size_t approx_length_of_decoded = length_of_string / 4 * 3;
    ret.reserve(approx_length_of_decoded);

    while (pos < length_of_string) {
        //
        // Iterate over encoded input string in chunks. The size of all
        // chunks except the last one is 4 bytes.
        //
        // The last chunk might be padded with equal signs or dots
        // in order to make it 4 bytes in size as well, but this
        // is not required as per RFC 2045.
        //
        // All chunks except the last one produce three output bytes.
        //
        // The last chunk produces at least one and up to three bytes.
        //

        int pos_of_char_1 = getPosOfChar(encoded_string[pos + 1]);
        if (pos_of_char_1 < 0)
            return false;

        //
        // Emit the first output byte that is produced in each chunk:
        //
        int pos_of_char_0 = getPosOfChar(encoded_string[pos + 0]);
        if (pos_of_char_0 < 0)
            return false;
        ret.push_back(static_cast<uint8_t>((pos_of_char_0 << 2) + ((pos_of_char_1 & 0x30) >> 4)));

        if ((pos + 2 < length_of_string) &&  // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
            encoded_string[pos + 2] != '=' &&
            encoded_string[pos + 2] != '.'  // accept URL-safe base 64 strings, too, so check for '.' also.
        ) {
            //
            // Emit a chunk's second byte (which might not be produced in the last chunk).
            //
            int pos_of_char_2 = getPosOfChar(encoded_string[pos + 2]);
            if (pos_of_char_2 < 0)
                return false;
            ret.push_back(static_cast<uint8_t>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

            if ((pos + 3 < length_of_string) &&
                encoded_string[pos + 3] != '=' &&
                encoded_string[pos + 3] != '.') {
                //
                // Emit a chunk's third byte (which might not be produced in the last chunk).
                //
                int pos_of_char_3 = getPosOfChar(encoded_string[pos + 3]);
                if (pos_of_char_3 < 0)
                    return false;
                ret.push_back(static_cast<uint8_t>(((pos_of_char_2 & 0x03) << 6) + pos_of_char_3));
            }
        }

        pos += 4;
    }

    return true;
}

std::vector<uint8_t> Base64::Decode(std::string const& s, bool remove_linebreaks) {
    std::vector<uint8_t> ret;
    if (_Decode(s, remove_linebreaks, ret))
        return ret;

    ret.resize(0);
    return ret;
}

std::string Base64::Encode(unsigned char const* bytes_to_encode, size_t in_len, bool url) {
    const size_t len_encoded = (in_len + 2) / 3 * 4;
    const unsigned char trailing_char = url ? '.' : '=';

    //
    // Choose set of base64 characters. They differ
    // for the last two positions, depending on the url
    // parameter.
    // A bool (as is the parameter url) is guaranteed
    // to evaluate to either 0 or 1 in C++ therefore,
    // the correct character set is chosen by subscripting
    // base64_chars with url.
    //
    const char* base64_chars = base64Chars(url ? 1 : 0);

    std::string ret;
    ret.reserve(len_encoded);

    unsigned int pos = 0;

    while (pos < in_len) {
        ret.push_back(base64_chars[(bytes_to_encode[pos + 0] & 0xfc) >> 2]);

        if (pos + 1 < in_len) {
            ret.push_back(base64_chars[((bytes_to_encode[pos + 0] & 0x03) << 4) + ((bytes_to_encode[pos + 1] & 0xf0) >> 4)]);

            if (pos + 2 < in_len) {
                ret.push_back(base64_chars[((bytes_to_encode[pos + 1] & 0x0f) << 2) + ((bytes_to_encode[pos + 2] & 0xc0) >> 6)]);
                ret.push_back(base64_chars[bytes_to_encode[pos + 2] & 0x3f]);
            }
            else {
                ret.push_back(base64_chars[(bytes_to_encode[pos + 1] & 0x0f) << 2]);
                ret.push_back(trailing_char);
            }
        }
        else {
            ret.push_back(base64_chars[(bytes_to_encode[pos + 0] & 0x03) << 4]);
            ret.push_back(trailing_char);
            ret.push_back(trailing_char);
        }

        pos += 3;
    }

    return ret;
}

#if ASHE_CPP_STANDARD_VER >= 201703L
std::string Base64::Encode(std::string_view s, bool url) {
    return _Encode(s, url);
}

std::string Base64::EncodeWithPEM(std::string_view s) {
    return _EncodeWithPEM(s);
}

std::string Base64::EncodeWithMIME(std::string_view s) {
    return _EncodeWithMIME(s);
}

std::vector<uint8_t> Base64::Decode(std::string_view s, bool remove_linebreaks) {
    std::vector<uint8_t> ret;
    if (_Decode(s, remove_linebreaks, ret))
        return ret;
    ret.resize(0);
    return ret;
}
#endif  // ASHE_CPP_STANDARD_VER >= 201703L

const char* Base64::base64Chars(int index) {
    //
    // Depending on the url parameter in base64_chars, one of
    // two sets of base64 characters needs to be chosen.
    // They differ in their last two characters.
    //
    static const char* base64_chars[2] = {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "+/",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "-_"};
    return base64_chars[index];
}



std::string Base64::insertLineBreaks(std::string str, size_t distance) {
    //
    // Provided by https://github.com/JomaCorpFX, adapted by me.
    //
    if (!str.length()) {
        return "";
    }

    size_t pos = distance;

    while (pos < str.size()) {
        str.insert(pos, "\n");
        pos += distance + 1;
    }

    return str;
}
}  // namespace ashe