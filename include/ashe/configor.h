/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2024 winsoft666 <winsoft666@outlook.com>.
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
#include "ashe/config.h"
#include "ashe/arch.h"
#include "ashe/macros.h"
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
    class ASHE_API Element {
        friend Configor;

       public:
        Element(const std::string& name, Configor* c);
        Element(Configor* c);
        Element(const Element& that);
        Element& operator=(const Element& that);

        void set(const std::string& value, const std::size_t i = 0);
        void set(const std::wstring& value, const std::size_t i = 0);
        void set(const int value, const std::size_t i = 0);
        void set(const float value, const std::size_t i = 0);
        void operator=(const std::string& value);
        void operator=(const std::wstring& value);
        void operator=(const int value);
        void operator=(const float value);
        void operator=(const double value);

        std::string getName();
        std::string getString(const std::size_t i = 0, const std::string& defaultValue = "");
        std::wstring getWString(const std::size_t i = 0, const std::wstring& defaultValue = L"");
        int getInt(const std::size_t i = 0, int defaultValue = 0);
        float getFloat(const std::size_t i = 0, float defaultValue = 0.f);

        Element& operator[](const std::string& name);
        Element& operator[](const std::wstring& name);

        std::size_t valueSize();
        std::size_t childrenSize();

        bool elementExists(const std::string& name);
        void removeElement(const std::string& name);
        void clearElements();
        void clearValues();

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
    bool reloadFile();

    // Only avaliable in file mode.
    bool saveFile();

    bool saveAsFile(const std::string& path);
    bool save(std::ostream& ss);

    Element& operator[](const std::string& name);
    Element& operator[](const std::wstring& name);

    bool elementExists(const std::string& name);
    void removeElement(const std::string& name);
    void clearElements();

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

    bool testString(const char c);
    std::recursive_mutex& mutex();
    bool accept(const std::string& token);
    bool expect(const std::string& token);
    void next();
    const std::string& get();

    void parseVar(std::vector<Element>& parent);
    void parseArray(Element& e);
    void parseObject(Element& e);

    void preprocess(std::istream* file);
    void tokenize();
    void parse();

    // output
    bool isNumeric(const std::string& value);
    void writeElement(std::ostream& file, const Element& element, const unsigned int tabs = 0);

    // top level element
    Element root_;

    std::string filePath_;

    ASHE_DISALLOW_COPY_AND_MOVE(Configor);
};
}  // namespace ashe
#endif  // !ASHE_CONFIGOR_HPP__
