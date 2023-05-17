/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 winsoft666 <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef ASHE_CONFIGOR_HPP__
#define ASHE_CONFIGOR_HPP__
#include "ashe/config.hpp"
#include "ashe/arch.hpp"
#include "ashe/macros.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <stdexcept>
#include <locale>
#include <mutex>

/*
---- Config Sample ----


# single line comment

string = "this is a string";
number = 123;
floating_number = 1.23;

# , without a following value will create an empty string in that place
# 3 for example has the index 4
array = [1, 2, , , 3, "4", 5.5] # the ; is optional after brackets

object = {
	name = "value within object";
	nr = 1;
	
	nested = {
		name = "nested object";
	}
	
	# careful, you cannot store values directly in objects
}

*/

namespace ashe {
// Thread-safe.
class ASHE_API Configor {
   public:
    ASHE_DISALLOW_COPY_MOVE(Configor);
    class Element {
        friend Configor;

       public:
        Element(const std::string& name, Configor* c);
        Element(Configor* c);
        Element(const Element& that);
        Element& operator=(const Element& that);

        void set(const std::string& value, const std::size_t i = 0) noexcept;
        void set(const std::wstring& value, const std::size_t i = 0) noexcept;
        void set(const int value, const std::size_t i = 0) noexcept;
        void set(const float value, const std::size_t i = 0) noexcept;
        void operator=(const std::string& value);
        void operator=(const std::wstring& value);
        void operator=(const int value);
        void operator=(const float value);
        void operator=(const double value);

        std::string getName() noexcept;
        std::string getString(const std::size_t i = 0, const std::string& defaultValue = "") noexcept;
        std::wstring getWString(const std::size_t i = 0, const std::wstring& defaultValue = L"") noexcept;
        int getInt(const std::size_t i = 0, int defaultValue = 0) noexcept;
        float getFloat(const std::size_t i = 0, float defaultValue = 0.f) noexcept;

        Element& operator[](const std::string& name);
        Element& operator[](const std::wstring& name);

        bool exist(const std::size_t i) noexcept;
        bool exists(const std::string& name) noexcept;
        void remove(const std::string& name) noexcept;
        void clear() noexcept;

       private:
        Configor* c_ = nullptr;
        std::string name_;
        std::vector<std::string> values_;
        std::vector<Element> children_;
    };

   public:
    Configor(const std::string& filePath);
    Configor(std::istream& ss);
    virtual ~Configor();

    // Only avaliable in file mode.
    bool reloadFile() noexcept;

    // Only avaliable in file mode.
    bool saveFile() noexcept;

    bool saveAsFile(const std::string& path) noexcept;
    bool save(std::ostream& ss) noexcept;

    Element& operator[](const std::string& name);
    Element& operator[](const std::wstring& name);

    bool exists(const std::string& name) noexcept;
    void remove(const std::string& name) noexcept;
    void clear() noexcept;

   private:
    struct Token {
        std::string token;
        std::size_t line, character;

        Token(const std::string& token, const std::size_t line, const std::size_t character) :
            token(token),
            line(line),
            character(character) {
        }
    };

    static const unsigned int TABS;
    static const std::set<char> delimeter;

    // preprocessor, tokenizer and parser
    std::string input_;
    std::size_t index_ = 0;
    std::vector<Token> tokens_;
    bool isstring_ = false;
    bool mask_ = false;
    std::recursive_mutex rwMutex_;

    bool testString(const char c) noexcept;
    std::recursive_mutex& mutex();
    bool accept(const std::string& token) noexcept;
    bool expect(const std::string& token) noexcept;
    void next() noexcept;
    const std::string& get() noexcept;

    void parseVar(std::vector<Element>& parent) noexcept;
    void parseArray(Element& e) noexcept;
    void parseObject(Element& e) noexcept;

    void preprocess(std::istream* file) noexcept;
    void tokenize() noexcept;
    void parse() noexcept;

    // output
    bool isNumeric(const std::string& value) noexcept;
    void writeElement(std::ostream& file, const Element& element, const unsigned int tabs = 0) noexcept;

    // top level element
    Element root_;

    std::string filePath_;
};
}  // namespace ashe
#endif  // !ASHE_CONFIGOR_HPP__
