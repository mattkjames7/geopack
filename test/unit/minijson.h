#ifndef TEST_UNIT_MINIJSON_H
#define TEST_UNIT_MINIJSON_H

#include <cctype>
#include <fstream>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace minijson {

struct Value;
using Object = std::map<std::string, Value>;
using Array = std::vector<Value>;

struct Value {
    std::variant<std::nullptr_t, double, std::string, Array, Object> v;

    bool isNull() const { return std::holds_alternative<std::nullptr_t>(v); }
    bool isNumber() const { return std::holds_alternative<double>(v); }
    bool isString() const { return std::holds_alternative<std::string>(v); }
    bool isArray() const { return std::holds_alternative<Array>(v); }
    bool isObject() const { return std::holds_alternative<Object>(v); }
    double asNumber() const { return std::get<double>(v); }
    const std::string &asString() const { return std::get<std::string>(v); }
    const Array &asArray() const { return std::get<Array>(v); }
    const Object &asObject() const { return std::get<Object>(v); }
};

class Parser {
  public:
    explicit Parser(std::string s) : s_(std::move(s)), i_(0) {}

    Value parse() {
        skipWs();
        Value out = parseValue();
        skipWs();
        if (i_ != s_.size()) throw std::runtime_error("trailing characters in json");
        return out;
    }

  private:
    std::string s_;
    size_t i_;

    void skipWs() {
        while (i_ < s_.size() && std::isspace(static_cast<unsigned char>(s_[i_]))) i_++;
    }

    char peek() const {
        if (i_ >= s_.size()) throw std::runtime_error("unexpected end of json");
        return s_[i_];
    }

    char take() {
        const char c = peek();
        i_++;
        return c;
    }

    void expect(char c) {
        if (take() != c) throw std::runtime_error("unexpected json token");
    }

    Value parseValue() {
        skipWs();
        const char c = peek();
        if (c == '{') return parseObject();
        if (c == '[') return parseArray();
        if (c == '"') return parseString();
        if (c == 'n') return parseNull();
        if (c == 't') return parseTrue();
        if (c == 'f') return parseFalse();
        if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber();
        throw std::runtime_error("unexpected token in json");
    }

    Value parseNull() {
        if (s_.substr(i_, 4) != "null") throw std::runtime_error("invalid null");
        i_ += 4;
        return Value{nullptr};
    }

    Value parseTrue() {
        if (s_.substr(i_, 4) != "true") throw std::runtime_error("invalid true");
        i_ += 4;
        return Value{1.0};
    }

    Value parseFalse() {
        if (s_.substr(i_, 5) != "false") throw std::runtime_error("invalid false");
        i_ += 5;
        return Value{0.0};
    }

    Value parseNumber() {
        size_t start = i_;
        if (s_[i_] == '-') i_++;
        while (i_ < s_.size() && std::isdigit(static_cast<unsigned char>(s_[i_]))) i_++;
        if (i_ < s_.size() && s_[i_] == '.') {
            i_++;
            while (i_ < s_.size() && std::isdigit(static_cast<unsigned char>(s_[i_]))) i_++;
        }
        if (i_ < s_.size() && (s_[i_] == 'e' || s_[i_] == 'E')) {
            i_++;
            if (i_ < s_.size() && (s_[i_] == '+' || s_[i_] == '-')) i_++;
            while (i_ < s_.size() && std::isdigit(static_cast<unsigned char>(s_[i_]))) i_++;
        }
        const std::string tok = s_.substr(start, i_ - start);
        char *end = nullptr;
        const double v = std::strtod(tok.c_str(), &end);
        if (end == tok.c_str()) throw std::runtime_error("invalid number token");
        return Value{v};
    }

    Value parseString() {
        expect('"');
        std::string out;
        while (true) {
            if (i_ >= s_.size()) throw std::runtime_error("unterminated string");
            char c = take();
            if (c == '"') break;
            if (c == '\\') {
                const char e = take();
                if (e == '"' || e == '\\' || e == '/') out.push_back(e);
                else if (e == 'b') out.push_back('\b');
                else if (e == 'f') out.push_back('\f');
                else if (e == 'n') out.push_back('\n');
                else if (e == 'r') out.push_back('\r');
                else if (e == 't') out.push_back('\t');
                else throw std::runtime_error("unsupported escape");
            } else {
                out.push_back(c);
            }
        }
        return Value{out};
    }

    Value parseArray() {
        expect('[');
        skipWs();
        Array out;
        if (peek() == ']') {
            take();
            return Value{out};
        }
        while (true) {
            out.push_back(parseValue());
            skipWs();
            const char c = take();
            if (c == ']') break;
            if (c != ',') throw std::runtime_error("expected ',' in array");
            skipWs();
        }
        return Value{out};
    }

    Value parseObject() {
        expect('{');
        skipWs();
        Object out;
        if (peek() == '}') {
            take();
            return Value{out};
        }
        while (true) {
            Value k = parseString();
            skipWs();
            expect(':');
            skipWs();
            out.emplace(k.asString(), parseValue());
            skipWs();
            const char c = take();
            if (c == '}') break;
            if (c != ',') throw std::runtime_error("expected ',' in object");
            skipWs();
        }
        return Value{out};
    }
};

inline Value parseFile(const std::string &path) {
    std::ifstream f(path);
    if (!f.good()) throw std::runtime_error("cannot open " + path);
    std::string data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return Parser(data).parse();
}

}  // namespace minijson

#endif
