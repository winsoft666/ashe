#include "ashe/config.h"

#include "ashe/configor.h"
#include "ashe/string_encode.h"

namespace ashe {

const unsigned int Configor::TABS = 4;
const std::set<char> Configor::delimeter = {'=', ';', '[', ']', '{', '}', ','};

// Element

Configor::Element::Element(const std::string& name, Configor* c) :
    name_(name),
    c_(c) {
}

Configor::Element::Element(const Configor::Element& that) {
    this->c_ = that.c_;
    this->name_ = that.name_;
    this->values_ = that.values_;
    this->children_ = that.children_;
}

Configor::Element::Element(Configor* c) :
    c_(c) {
}

void Configor::Element::set(const std::string& value, const std::size_t i) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    if (i >= values_.size()) {
        values_.resize(i + 1);
    }

    values_[i] = value;

    c_->saveFile();
}

void Configor::Element::set(const std::wstring& value, const std::size_t i) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    if (i >= values_.size()) {
        values_.resize(i + 1);
    }

    values_[i] = UnicodeToUtf8(value);

    c_->saveFile();
}

void Configor::Element::set(const int value, const std::size_t i) {
    set(std::to_string(value), i);
}

void Configor::Element::set(const float value, const std::size_t i) {
    set(std::to_string(value), i);
}

void Configor::Element::operator=(const std::string& value) {
    set(value);
}

void Configor::Element::operator=(const std::wstring& value) {
    set(value);
}

void Configor::Element::operator=(const int value) {
    set(value);
}

void Configor::Element::operator=(const float value) {
    set(value);
}

void Configor::Element::operator=(const double value) {
    set(float(value));
}

Configor::Element& Configor::Element::operator=(const Configor::Element& that) {
    this->name_ = that.name_;
    this->values_ = that.values_;
    this->children_ = that.children_;
    this->c_ = that.c_;

    return *this;
}

std::string Configor::Element::getName() {
    return name_;
}

std::string Configor::Element::getString(const std::size_t i, const std::string& defaultValue) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    if (i >= values_.size()) {
        values_.resize(i + 1);
        values_[i] = defaultValue;
    }

    return values_[i];
}

std::wstring Configor::Element::getWString(const std::size_t i, const std::wstring& defaultValue) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    if (i >= values_.size()) {
        values_.resize(i + 1);
        values_[i] = UnicodeToUtf8(defaultValue);
    }

    return Utf8ToUnicode(values_[i]);
}

int Configor::Element::getInt(const std::size_t i, int defaultValue) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    if (i >= values_.size())
        return defaultValue;

    int ret = defaultValue;
    try {
        const std::string& s = getString(i);
        if (!s.empty())
            ret = std::stoi(s);
    } catch (...) {
        ret = defaultValue;
    }
    return ret;
}

float Configor::Element::getFloat(const std::size_t i, float defaultValue) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    if (i >= values_.size())
        return defaultValue;

    float ret = defaultValue;
    try {
        const std::string& s = getString(i);
        if (!s.empty())
            ret = std::stof(s);
    } catch (...) {
        ret = defaultValue;
    }
    return ret;
}

Configor::Element& Configor::Element::operator[](const std::string& name) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    for (auto i = children_.begin(); i != children_.end(); i++) {
        if ((*i).name_ == name) {
            return *i;
        }
    }

    children_.push_back(Element(name, c_));

    return children_.back();
}

Configor::Element& Configor::Element::operator[](const std::wstring& name) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    std::string nameu8 = UnicodeToUtf8(name);
    for (auto i = children_.begin(); i != children_.end(); i++) {
        if ((*i).name_ == nameu8) {
            return *i;
        }
    }

    children_.push_back(Element(nameu8, c_));

    return children_.back();
}

std::size_t Configor::Element::valueSize() {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    return values_.size();
}

std::size_t Configor::Element::childrenSize() {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    return children_.size();
}

bool Configor::Element::elementExists(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    for (const auto& e : children_) {
        if (e.name_ == name) {
            return true;
        }
    }

    return false;
}

void Configor::Element::removeElement(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    for (auto i = children_.cbegin(); i != children_.cend(); i++) {
        if ((*i).name_ == name) {
            children_.erase(i);

            c_->saveFile();
            return;
        }
    }
}

void Configor::Element::clearElements() {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    children_.clear();

    c_->saveFile();
}

void Configor::Element::clearValues() {
    std::lock_guard<std::recursive_mutex> lg(c_->mutex());
    values_.clear();

    c_->saveFile();
}

// Configor

bool Configor::testString(const char c) {
    if (isstring_ && c == '\\') {  // allow mask in strings
        mask_ = !mask_;

        if (mask_) {
            return true;
        }
    }

    if (!mask_ && (c == '"' || c == '\'')) {  // strings " or '
        isstring_ = !isstring_;
    }

    mask_ = false;  // reset mask

    return false;
}

std::recursive_mutex& Configor::mutex() {
    return rwMutex_;
}

bool Configor::accept(const std::string& token) {
    return token == get();
}

bool Configor::expect(const std::string& token) {
    if (token != get()) {
        return false;
    }

    next();
    return true;
}

void Configor::next() {
    if (index_ < tokens_.size() - 1) {
        index_++;
    }
}

const std::string& Configor::get() {
    return tokens_[index_].token;
}

void Configor::parseVar(std::vector<Element>& parent) {
    Element e(get(), this);
    next();
    if (!expect("="))
        return;

    if (accept("[")) {
        parseArray(e);
    }
    else if (accept("{")) {
        parseObject(e);
    }
    else {
        e.values_.push_back(get());
        next();
        if (!expect(";"))
            return;
    }

    parent.push_back(e);
}

void Configor::parseArray(Element& e) {
    if (!expect("["))
        return;

    while (!accept("]")) {
        e.values_.push_back(get());
        next();

        if (accept(",")) {
            next();
        }

        while (accept(",")) {
            e.values_.push_back("");
            next();
        }
    }

    if (!expect("]"))
        return;

    if (accept(";")) {  // optional
        next();
    }
}

void Configor::parseObject(Element& e) {
    if (!expect("{"))
        return;

    while (!accept("}")) {
        parseVar(e.children_);
    }

    if (!expect("}"))
        return;

    if (accept(";")) {  // optional
        next();
    }
}

void Configor::preprocess(std::istream* file) {
    if (!file)
        return;

    input_ = "";

    std::string line;
    isstring_ = false;
    mask_ = false;

    while (std::getline(*file, line)) {
        for (auto c : line) {
            if (!isstring_ && (c == ' ' || c == '\t')) {  // skip white space
                continue;
            }

            if (!isstring_ && c == '#') {  // skip comments
                break;
            }

            testString(c);

            input_ += c;
        }

        if (isstring_) {
            input_ += '\n';
        }
    }
}

void Configor::tokenize() {
    tokens_.clear();

    auto end = delimeter.end();
    std::string token, deli;
    isstring_ = false;
    mask_ = false;
    std::size_t line = 1;
    std::size_t character = 0;

    for (auto c : input_) {
        if (testString(c)) {
            continue;
        }

        // count lines and character but skip new line
        if (c == '\n') {
            line++;
            character = 0;

            continue;
        }

        if (!isstring_ && delimeter.find(c) != end) {
            // cut " if string
            if (token.length() && token.at(0) == '"' && token.at(token.length() - 1) == '"') {
                token = token.substr(1, token.length() - 2);
            }

            // store token
            if (!token.empty()) {
                tokens_.push_back(Token(token, line, character));
                token = "";
            }

            // store delimeter
            deli = c;
            tokens_.push_back(Token(deli, line, character));

            continue;
        }

        token += c;
        character = 0;
    }
}

void Configor::parse() {
    index_ = 0;
    root_.clearElements();

    if (tokens_.size() > 0) {
        while (index_ != tokens_.size() - 1) {
            parseVar(root_.children_);
        }
    }
}

bool Configor::isNumeric(const std::string& value) {
    for (auto c : value) {
        if (!std::isdigit(c, std::locale()) && c != '.' && c != '-') {
            return false;
        }
    }

    return true;
}

void Configor::writeElement(std::ostream& file, const Element& element, const unsigned int tabs) {
    if (!element.values_.size() && !element.children_.size()) {
        return;
    }

    for (unsigned int i = 0; i < tabs; i++) {
        file << " ";
    }

    bool isnumeric = false;

    file << element.name_ << " = ";

    if (element.children_.size() > 0) {
        file << "{\n";

        for (const auto& e : element.children_) {
            writeElement(file, e, tabs + TABS);
        }

        for (unsigned int i = 0; i < tabs; i++) {
            file << " ";
        }

        file << "}";
    }
    else if (element.values_.size() == 1) {
        if (isNumeric(element.values_[0])) {
            file << element.values_[0] << ";";
        }
        else {
            file << "\"" << element.values_[0] << "\";";
        }
    }
    else {
        file << "[";

        for (std::size_t i = 0; i < element.values_.size(); i++) {
            isnumeric = isNumeric(element.values_[i]);

            if (!isnumeric) {
                file << "\"";
            }

            file << element.values_[i];

            if (!isnumeric) {
                file << "\"";
            }

            if (i != element.values_.size() - 1) {
                file << ", ";
            }
        }

        file << "]";
    }

    file << "\n";
}

Configor::Configor(const std::string& filePath) :
    filePath_(filePath),
    root_(this) {
    reloadFile();
}

Configor::Configor(std::istream& ss) :
    root_(this) {
    preprocess(&ss);

    if (!input_.empty()) {
        tokenize();
        parse();
    }
}

Configor::~Configor() {
}

bool Configor::reloadFile() {
    std::lock_guard<std::recursive_mutex> lg(rwMutex_);
    try {
        if (filePath_.empty())
            return false;

        std::ifstream file(filePath_);

        if (!file.good()) {
            file.close();
            return false;
        }

        preprocess(&file);
        file.close();

        if (!input_.empty()) {
            tokenize();
            parse();
        }
    } catch (std::exception& e) {
        (e);
        return false;
    }

    return true;
}

bool Configor::saveFile() {
    std::lock_guard<std::recursive_mutex> lg(rwMutex_);
    try {
        if (filePath_.empty())
            return false;
        std::ofstream file(filePath_);

        if (!file.good()) {
            file.close();
            return false;
        }

        for (const auto& e : root_.children_) {
            writeElement(file, e);
        }

        file.close();
    } catch (std::exception& e) {
        (e);
        return false;
    }

    return true;
}

bool Configor::saveAsFile(const std::string& path) {
    std::lock_guard<std::recursive_mutex> lg(rwMutex_);
    try {
        if (path.empty())
            return false;

        std::lock_guard<std::recursive_mutex> lg(rwMutex_);
        std::ofstream file(path);

        if (!file.good()) {
            file.close();
            return false;
        }

        for (const auto& e : root_.children_) {
            writeElement(file, e);
        }

        file.close();
    } catch (std::exception& e) {
        (e);
        return false;
    }

    return true;
}

bool Configor::save(std::ostream& ss) {
    std::lock_guard<std::recursive_mutex> lg(rwMutex_);
    try {
        for (const auto& e : root_.children_) {
            writeElement(ss, e);
        }
    } catch (std::exception& e) {
        (e);
        return false;
    }
    return true;
}

Configor::Element& Configor::operator[](const std::string& name) {
    return root_[name];
}

Configor::Element& Configor::operator[](const std::wstring& name) {
    std::string nameu8 = UnicodeToUtf8(name);
    return root_[nameu8];
}

bool Configor::elementExists(const std::string& name) {
    return root_.elementExists(name);
}

void Configor::removeElement(const std::string& name) {
    root_.removeElement(name);
}

void Configor::clearElements() {
    root_.clearElements();
}
}  // namespace ashe