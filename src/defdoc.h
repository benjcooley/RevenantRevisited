// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Reveneant Revisited 2026                         *
// *                defdoc.h - DEF Document Parser / Writer                *
// *************************************************************************
//
// A compact, project-native config format with a JSON-shaped data model.
// It accepts classic Revenant DEF-style blocks:
//
//   AREA "The Forest"
//   BEGIN
//     LEVEL 10
//     AMBCOLOR 250, 250, 250
//   END
//
// and a stricter modern style:
//
//   RENDERPOLICY "Water" {
//     pass = "transparent_world"
//     z_write = false
//     color = [0.2, 0.4, 1.0]
//   }
//
// Data model:
//   null | bool | int | double | string | array | object
//
// Repeated blocks are stored as arrays under their block key. Block labels are
// stored in the child object as "$name". Keys are kept as authored, while
// lookup helpers are case-insensitive to match legacy DEF behavior.
// *************************************************************************

#pragma once

#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace defdoc {

enum class WriteStyle
{
    Auto,
    Inline,
    Block,
};

enum class AssignStyle
{
    Equals,
    Space,
};

struct WriteRule
{
    std::string path;
    WriteStyle style = WriteStyle::Auto;
    bool has_assign_style = false;
    AssignStyle assign_style = AssignStyle::Equals;
};

struct WriteOptions
{
    std::vector<WriteRule> rules;
    AssignStyle assign_style = AssignStyle::Equals;

    WriteOptions& force_inline(std::string path)
    {
        WriteRule rule;
        rule.path = std::move(path);
        rule.style = WriteStyle::Inline;
        rules.push_back(std::move(rule));
        return *this;
    }

    WriteOptions& force_block(std::string path)
    {
        WriteRule rule;
        rule.path = std::move(path);
        rule.style = WriteStyle::Block;
        rules.push_back(std::move(rule));
        return *this;
    }

    WriteOptions& use_equals(bool on = true)
    {
        assign_style = on ? AssignStyle::Equals : AssignStyle::Space;
        return *this;
    }

    WriteOptions& use_space_assignment()
    {
        assign_style = AssignStyle::Space;
        return *this;
    }

    WriteOptions& assign_equals(std::string path)
    {
        WriteRule rule;
        rule.path = std::move(path);
        rule.has_assign_style = true;
        rule.assign_style = AssignStyle::Equals;
        rules.push_back(std::move(rule));
        return *this;
    }

    WriteOptions& assign_space(std::string path)
    {
        WriteRule rule;
        rule.path = std::move(path);
        rule.has_assign_style = true;
        rule.assign_style = AssignStyle::Space;
        rules.push_back(std::move(rule));
        return *this;
    }
};

struct ParseOptions
{
    bool preserve_comments = true;
};

class Error : public std::runtime_error
{
public:
    Error(int line, int col, const std::string& msg)
        : std::runtime_error("DEF parse error at " + std::to_string(line) + ":" +
                             std::to_string(col) + ": " + msg),
          line_(line), col_(col) {}

    int line() const { return line_; }
    int col() const { return col_; }

private:
    int line_;
    int col_;
};

struct Metadata
{
    std::vector<std::string> leading_comments;
    int leading_blank_lines = 0;
};

class Node
{
public:
    enum class Type { Null, Bool, Int, Double, String, Array, Object };

    using Array = std::vector<Node>;
    using Object = std::map<std::string, Node>;

    Node() = default;
    Node(std::nullptr_t) : value_(nullptr) {}
    Node(bool v) : value_(v) {}
    Node(int v) : value_(int64_t(v)) {}
    Node(int64_t v) : value_(v) {}
    Node(double v) : value_(v) {}
    Node(const char* v) : value_(std::string(v ? v : "")) {}
    Node(std::string v) : value_(std::move(v)) {}
    Node(Array v) : value_(std::move(v)) {}
    Node(Object v) : value_(std::move(v)) {}

    Type type() const
    {
        switch (value_.index())
        {
        case 0: return Type::Null;
        case 1: return Type::Bool;
        case 2: return Type::Int;
        case 3: return Type::Double;
        case 4: return Type::String;
        case 5: return Type::Array;
        default: return Type::Object;
        }
    }

    bool is_null() const { return std::holds_alternative<std::nullptr_t>(value_); }
    bool is_bool() const { return std::holds_alternative<bool>(value_); }
    bool is_int() const { return std::holds_alternative<int64_t>(value_); }
    bool is_double() const { return std::holds_alternative<double>(value_); }
    bool is_number() const { return is_int() || is_double(); }
    bool is_string() const { return std::holds_alternative<std::string>(value_); }
    bool is_array() const { return std::holds_alternative<Array>(value_); }
    bool is_object() const { return std::holds_alternative<Object>(value_); }

    Array& as_array()
    {
        if (!is_array()) value_ = Array{};
        return std::get<Array>(value_);
    }
    const Array& as_array() const
    {
        static const Array empty;
        return is_array() ? std::get<Array>(value_) : empty;
    }

    Object& as_object()
    {
        if (!is_object()) value_ = Object{};
        return std::get<Object>(value_);
    }
    const Object& as_object() const
    {
        static const Object empty;
        return is_object() ? std::get<Object>(value_) : empty;
    }

    bool as_bool(bool fallback = false) const
    {
        if (auto p = std::get_if<bool>(&value_)) return *p;
        if (auto p = std::get_if<int64_t>(&value_)) return *p != 0;
        if (auto p = std::get_if<std::string>(&value_))
        {
            std::string s = lower(*p);
            if (s == "true" || s == "yes" || s == "on" || s == "1") return true;
            if (s == "false" || s == "no" || s == "off" || s == "0") return false;
        }
        return fallback;
    }

    int64_t as_int(int64_t fallback = 0) const
    {
        if (auto p = std::get_if<int64_t>(&value_)) return *p;
        if (auto p = std::get_if<double>(&value_)) return static_cast<int64_t>(*p);
        if (auto p = std::get_if<bool>(&value_)) return *p ? 1 : 0;
        if (auto p = std::get_if<std::string>(&value_))
        {
            char* end = nullptr;
            const long long v = std::strtoll(p->c_str(), &end, 0);
            if (end && *end == '\0') return static_cast<int64_t>(v);
        }
        return fallback;
    }

    double as_double(double fallback = 0.0) const
    {
        if (auto p = std::get_if<double>(&value_)) return *p;
        if (auto p = std::get_if<int64_t>(&value_)) return static_cast<double>(*p);
        if (auto p = std::get_if<bool>(&value_)) return *p ? 1.0 : 0.0;
        if (auto p = std::get_if<std::string>(&value_))
        {
            char* end = nullptr;
            const double v = std::strtod(p->c_str(), &end);
            if (end && *end == '\0') return v;
        }
        return fallback;
    }

    std::string as_string(const std::string& fallback = {}) const
    {
        if (auto p = std::get_if<std::string>(&value_)) return *p;
        if (auto p = std::get_if<bool>(&value_)) return *p ? "true" : "false";
        if (auto p = std::get_if<int64_t>(&value_)) return std::to_string(*p);
        if (auto p = std::get_if<double>(&value_))
        {
            std::ostringstream ss;
            ss << *p;
            return ss.str();
        }
        return fallback;
    }

    Node& operator[](const std::string& key) { return as_object()[key]; }

    const Node& operator[](const std::string& key) const
    {
        static const Node null_node;
        if (!is_object()) return null_node;
        const Object& obj = std::get<Object>(value_);
        auto it = obj.find(key);
        if (it != obj.end()) return it->second;
        for (const auto& kv : obj)
            if (iequals(kv.first, key))
                return kv.second;
        return null_node;
    }

    bool contains(const std::string& key) const
    {
        if (!is_object()) return false;
        const Object& obj = std::get<Object>(value_);
        if (obj.find(key) != obj.end()) return true;
        for (const auto& kv : obj)
            if (iequals(kv.first, key))
                return true;
        return false;
    }

    std::string get_string(const std::string& key, const std::string& fallback = {}) const
        { return (*this)[key].as_string(fallback); }
    int64_t get_int(const std::string& key, int64_t fallback = 0) const
        { return (*this)[key].as_int(fallback); }
    double get_double(const std::string& key, double fallback = 0.0) const
        { return (*this)[key].as_double(fallback); }
    bool get_bool(const std::string& key, bool fallback = false) const
        { return (*this)[key].as_bool(fallback); }

    std::vector<const Node*> blocks(const std::string& key) const
    {
        std::vector<const Node*> out;
        if (!is_object()) return out;
        for (const auto& kv : as_object())
        {
            if (!iequals(kv.first, key))
                continue;
            const Node& n = kv.second;
            if (n.is_array())
                for (const Node& child : n.as_array())
                    out.push_back(&child);
            else if (!n.is_null())
                out.push_back(&n);
        }
        return out;
    }

    static bool iequals(const std::string& a, const std::string& b)
    {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i)
            if (std::tolower((unsigned char)a[i]) != std::tolower((unsigned char)b[i]))
                return false;
        return true;
    }

    static std::string lower(std::string s)
    {
        for (char& c : s)
            c = (char)std::tolower((unsigned char)c);
        return s;
    }

private:
    std::variant<std::nullptr_t, bool, int64_t, double, std::string, Array, Object> value_{nullptr};
};

namespace detail {

enum class TokKind {
    Eof, Newline, Comment, Ident, String, Number, Begin, End,
    Equal, Comma, LBracket, RBracket, LBrace, RBrace
};

struct Token {
    TokKind kind = TokKind::Eof;
    std::string text;
    int line = 1;
    int col = 1;
};

class Lexer
{
public:
    explicit Lexer(std::string src) : src_(preprocess(std::move(src))) {}

    std::vector<Token> lex()
    {
        std::vector<Token> out;
        for (;;)
        {
            Token t = next();
            out.push_back(t);
            if (t.kind == TokKind::Eof) break;
        }
        return out;
    }

private:
    static std::string preprocess(std::string src)
    {
        std::string out;
        out.reserve(src.size());
        for (size_t i = 0; i < src.size(); ++i)
        {
            if (src[i] == '\\' && i + 1 < src.size() && src[i + 1] == '\n')
            {
                out.push_back(' ');
                ++i;
                while (i + 1 < src.size() && (src[i + 1] == ' ' || src[i + 1] == '\t'))
                    ++i;
                continue;
            }
            if (src[i] == '\\' && i + 2 < src.size() && src[i + 1] == '\r' && src[i + 2] == '\n')
            {
                out.push_back(' ');
                i += 2;
                while (i + 1 < src.size() && (src[i + 1] == ' ' || src[i + 1] == '\t'))
                    ++i;
                continue;
            }
            out.push_back(src[i]);
        }
        return out;
    }

    Token next()
    {
        for (;;)
        {
            if (pos_ >= src_.size()) return make(TokKind::Eof);
            const char c = src_[pos_];
            if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }
            if (c == '\n') { Token t = make(TokKind::Newline); advance(); return t; }
            if (c == '#') return comment_token(1);
            if (c == '/' && pos_ + 1 < src_.size() && src_[pos_ + 1] == '/') return comment_token(2);
            break;
        }

        const char c = src_[pos_];
        if (c == '=') { Token t = make(TokKind::Equal); advance(); return t; }
        if (c == ',') { Token t = make(TokKind::Comma); advance(); return t; }
        if (c == '[') { Token t = make(TokKind::LBracket); advance(); return t; }
        if (c == ']') { Token t = make(TokKind::RBracket); advance(); return t; }
        if (c == '{') { Token t = make(TokKind::LBrace); advance(); return t; }
        if (c == '}') { Token t = make(TokKind::RBrace); advance(); return t; }
        if (c == '"') return string_token();
        if (std::isdigit((unsigned char)c) || c == '-' || c == '+') return number_or_ident();
        return ident_token();
    }

    Token make(TokKind kind, std::string text = {}) const
    {
        return Token{kind, std::move(text), line_, col_};
    }

    void advance()
    {
        if (src_[pos_] == '\n') { ++line_; col_ = 1; }
        else ++col_;
        ++pos_;
    }

    Token comment_token(int marker_len)
    {
        Token t = make(TokKind::Comment);
        for (int i = 0; i < marker_len; ++i)
            advance();
        while (pos_ < src_.size() && (src_[pos_] == ' ' || src_[pos_] == '\t'))
            advance();
        while (pos_ < src_.size() && src_[pos_] != '\n')
        {
            t.text.push_back(src_[pos_]);
            advance();
        }
        while (!t.text.empty() && (t.text.back() == ' ' || t.text.back() == '\t' || t.text.back() == '\r'))
            t.text.pop_back();
        return t;
    }

    Token string_token()
    {
        Token t = make(TokKind::String);
        advance(); // opening quote
        while (pos_ < src_.size())
        {
            char c = src_[pos_];
            if (c == '"') { advance(); return t; }
            if (c == '\\')
            {
                advance();
                if (pos_ >= src_.size()) break;
                c = src_[pos_];
                switch (c)
                {
                case 'n': t.text.push_back('\n'); break;
                case 't': t.text.push_back('\t'); break;
                case 'r': t.text.push_back('\r'); break;
                case '\\': t.text.push_back('\\'); break;
                case '"': t.text.push_back('"'); break;
                default: t.text.push_back(c); break;
                }
                advance();
                continue;
            }
            t.text.push_back(c);
            advance();
        }
        throw Error(t.line, t.col, "unterminated string");
    }

    Token ident_token()
    {
        Token t = make(TokKind::Ident);
        while (pos_ < src_.size())
        {
            const char c = src_[pos_];
            if (std::isspace((unsigned char)c) || c == '=' || c == ',' ||
                c == '[' || c == ']' || c == '{' || c == '}' || c == '#')
                break;
            if (c == '/' && pos_ + 1 < src_.size() && src_[pos_ + 1] == '/') break;
            t.text.push_back(c);
            advance();
        }
        if (Node::iequals(t.text, "BEGIN")) t.kind = TokKind::Begin;
        else if (Node::iequals(t.text, "END")) t.kind = TokKind::End;
        return t;
    }

    Token number_or_ident()
    {
        Token t = ident_token();
        char* end = nullptr;
        std::strtod(t.text.c_str(), &end);
        if (end && *end == '\0' && t.text != "+" && t.text != "-")
            t.kind = TokKind::Number;
        return t;
    }

    std::string src_;
    size_t pos_ = 0;
    int line_ = 1;
    int col_ = 1;
};

class Parser
{
public:
    explicit Parser(std::vector<Token> tokens, ParseOptions options = {})
        : tokens_(std::move(tokens)), options_(options) {}

    Node parse_document()
    {
        Node root(Node::Object{});
        while (!at(TokKind::Eof))
        {
            pending_meta_ = collect_leading_meta();
            if (at(TokKind::Eof)) break;
            parse_statement(root.as_object());
        }
        return root;
    }

    const std::map<std::string, Metadata>& metadata() const { return metadata_; }

private:
    const Token& peek(size_t n = 0) const
    {
        const size_t idx = pos_ + n;
        return tokens_[idx < tokens_.size() ? idx : tokens_.size() - 1];
    }

    bool at(TokKind k, size_t n = 0) const { return peek(n).kind == k; }

    Token take()
    {
        if (pos_ < tokens_.size()) return tokens_[pos_++];
        return tokens_.back();
    }

    void expect(TokKind k, const char* what)
    {
        if (!at(k))
            throw Error(peek().line, peek().col, std::string("expected ") + what);
        take();
    }

    void skip_newlines()
    {
        while (at(TokKind::Newline))
            take();
    }

    Metadata collect_leading_meta()
    {
        Metadata meta;
        int newline_run = 0;
        while (at(TokKind::Newline) || at(TokKind::Comment))
        {
            if (at(TokKind::Newline))
            {
                take();
                ++newline_run;
                continue;
            }
            if (newline_run > 1)
                meta.leading_blank_lines += newline_run - 1;
            newline_run = 0;
            meta.leading_comments.push_back(take().text);
        }
        if (newline_run > 1)
            meta.leading_blank_lines += newline_run - 1;
        return meta;
    }

    void parse_statement(Node::Object& obj)
    {
        Metadata meta = std::move(pending_meta_);
        if (!at(TokKind::Ident))
        {
            if (is_value_start(peek().kind))
            {
                Node row = parse_legacy_value_list();
                append_anonymous_item(obj, std::move(row));
                return;
            }
            throw Error(peek().line, peek().col, "expected identifier");
        }
        const Token key = take();

        // Support legacy multi-word keys before '=':
        //   Unique Type ID = 0x0d22
        if (at(TokKind::Ident))
        {
            size_t scan = pos_;
            bool only_idents = true;
            while (scan < tokens_.size() && tokens_[scan].kind != TokKind::Newline &&
                   tokens_[scan].kind != TokKind::Comment &&
                   tokens_[scan].kind != TokKind::Eof)
            {
                if (tokens_[scan].kind == TokKind::Equal)
                    break;
                if (tokens_[scan].kind != TokKind::Ident)
                    only_idents = false;
                ++scan;
            }
            if (scan < tokens_.size() && tokens_[scan].kind == TokKind::Equal && only_idents)
            {
                std::string full_key = key.text;
                while (pos_ < scan)
                {
                    full_key += " ";
                    full_key += take().text;
                }
                take(); // '='
                if (!is_value_start(peek().kind))
                    throw Error(peek().line, peek().col, "expected value after =");
                obj[full_key] = parse_value();
                store_metadata(child_metadata_key(full_key), std::move(meta));
                return;
            }
        }

        size_t block_pos = pos_;
        bool saw_newline_before_block = false;
        while (block_pos < tokens_.size())
        {
            const TokKind k = tokens_[block_pos].kind;
            if (k == TokKind::Comment) { ++block_pos; continue; }
            if (k == TokKind::Newline) { saw_newline_before_block = true; ++block_pos; continue; }
            if (k == TokKind::Begin || k == TokKind::LBrace) break;
            if (saw_newline_before_block || k == TokKind::Equal || k == TokKind::Eof ||
                k == TokKind::End || k == TokKind::RBrace) break;
            ++block_pos;
        }

        const bool is_block = block_pos < tokens_.size() &&
            (tokens_[block_pos].kind == TokKind::Begin || tokens_[block_pos].kind == TokKind::LBrace);
        if (is_block)
        {
            Node block(Node::Object{});
            Node label;
            bool has_label = false;
            bool label_checked = false;
            bool parsed_label = false;
            while (pos_ < block_pos)
            {
                if (at(TokKind::Newline) || at(TokKind::Comment)) { take(); continue; }
                const bool first_is_label = !label_checked &&
                    (at(TokKind::String) || at(TokKind::Number) ||
                     (at(TokKind::Ident) && (peek(1).kind == TokKind::Begin ||
                                             peek(1).kind == TokKind::LBrace ||
                                             peek(1).kind == TokKind::Newline ||
                                             peek(1).kind == TokKind::Comment)));
                if (first_is_label)
                {
                    label = parse_value();
                    block["$name"] = label;
                    has_label = true;
                    label_checked = true;
                    parsed_label = true;
                    continue;
                }
                if (!parsed_label)
                {
                    Node::Array args;
                    while (pos_ < block_pos)
                    {
                        if (at(TokKind::Newline) || at(TokKind::Comment) || at(TokKind::Comma)) { take(); continue; }
                        args.push_back(parse_value());
                    }
                    block["$args"] = std::move(args);
                    break;
                }
                label_checked = true;
                if (!at(TokKind::Ident))
                    throw Error(peek().line, peek().col, "expected block header key");
                const Token hkey = take();
                if (at(TokKind::Equal)) take();
                if (!is_value_start(peek().kind))
                    throw Error(peek().line, peek().col, "expected block header value");
                block[hkey.text] = parse_value();
                if (at(TokKind::Comma)) take();
            }
            while (pos_ < block_pos)
                take();
            const std::string block_path = block_metadata_key(key.text, has_label ? &label : nullptr);
            store_metadata(block_path, std::move(meta));
            path_stack_.push_back(block_path);
            parse_block_body(block.as_object());
            path_stack_.pop_back();
            append_repeated(obj, key.text, block);
            return;
        }

        bool used_equal = false;
        if (at(TokKind::Equal))
        {
            take();
            used_equal = true;
        }

        std::vector<Node> values;
        if (used_equal)
        {
            if (!is_value_start(peek().kind))
                throw Error(peek().line, peek().col, "expected value after =");
            values.push_back(parse_value());
        }
        else
        {
            Node row = parse_legacy_value_list();
            if (row.is_array())
                values = row.as_array();
            else if (!row.is_null())
                values.push_back(row);
        }
        if (values.empty())
        {
            obj[key.text] = true;
            store_metadata(child_metadata_key(key.text), std::move(meta));
        }
        else if (values.size() == 1)
        {
            obj[key.text] = std::move(values[0]);
            store_metadata(child_metadata_key(key.text), std::move(meta));
        }
        else
        {
            obj[key.text] = Node::Array(std::move(values));
            store_metadata(child_metadata_key(key.text), std::move(meta));
        }
    }

    void parse_block_body(Node::Object& obj)
    {
        const bool brace = at(TokKind::LBrace);
        take();
        while (!at(TokKind::Eof))
        {
            pending_meta_ = collect_leading_meta();
            if (brace && at(TokKind::RBrace)) { take(); return; }
            if (!brace && at(TokKind::End)) { take(); return; }
            parse_statement(obj);
        }
        throw Error(peek().line, peek().col, brace ? "missing }" : "missing END");
    }

    static bool is_value_start(TokKind k)
    {
        return k == TokKind::Ident || k == TokKind::String || k == TokKind::Number ||
               k == TokKind::LBracket || k == TokKind::LBrace;
    }

    Node parse_value()
    {
        Token t = take();
        switch (t.kind)
        {
        case TokKind::String: return t.text;
        case TokKind::Number: return parse_number(t);
        case TokKind::Ident:
        {
            if (Node::iequals(t.text, "true")) return true;
            if (Node::iequals(t.text, "false")) return false;
            if (Node::iequals(t.text, "null")) return nullptr;
            return t.text;
        }
        case TokKind::LBracket: return parse_array(t);
        case TokKind::LBrace: return parse_inline_object(t);
        default:
            throw Error(t.line, t.col, "expected value");
        }
    }

    Node parse_number(const Token& t)
    {
        if (t.text.find_first_of(".eE") != std::string::npos)
            return std::strtod(t.text.c_str(), nullptr);
        return static_cast<int64_t>(std::strtoll(t.text.c_str(), nullptr, 0));
    }

    Node parse_legacy_value_list()
    {
        std::vector<Node> values;
        while (!at(TokKind::Eof) && !at(TokKind::Newline) &&
               !at(TokKind::Comment) && !at(TokKind::End) && !at(TokKind::RBrace))
        {
            if (at(TokKind::Comma)) { take(); continue; }
            values.push_back(parse_value());
        }
        if (values.empty())
            return nullptr;
        if (values.size() == 1)
            return std::move(values[0]);
        return Node::Array(std::move(values));
    }

    Node parse_array(const Token&)
    {
        Node::Array arr;
        while (!at(TokKind::Eof))
        {
            collect_leading_meta();
            if (at(TokKind::RBracket)) { take(); return arr; }
            if (at(TokKind::Comma)) { take(); continue; }
            arr.push_back(parse_value());
        }
        throw Error(peek().line, peek().col, "missing ]");
    }

    Node parse_inline_object(const Token&)
    {
        Node obj(Node::Object{});
        collect_leading_meta();
        if (!at(TokKind::RBrace) && !at(TokKind::Ident))
        {
            Node::Array arr;
            while (!at(TokKind::Eof))
            {
                collect_leading_meta();
                if (at(TokKind::RBrace)) { take(); return arr; }
                if (at(TokKind::Comma)) { take(); continue; }
                arr.push_back(parse_value());
            }
            throw Error(peek().line, peek().col, "missing }");
        }
        while (!at(TokKind::Eof))
        {
            collect_leading_meta();
            if (at(TokKind::RBrace)) { take(); return obj; }
            if (at(TokKind::Comma)) { take(); continue; }
            if (!at(TokKind::Ident))
                throw Error(peek().line, peek().col, "expected inline object key");
            const Token key = take();
            if (at(TokKind::Equal))
                take();
            if (!is_value_start(peek().kind))
                throw Error(peek().line, peek().col, "expected inline object value");
            obj[key.text] = parse_value();
            if (at(TokKind::Comma)) take();
        }
        throw Error(peek().line, peek().col, "missing }");
    }

    static void append_repeated(Node::Object& obj, const std::string& key, Node value)
    {
        auto it = obj.find(key);
        if (it == obj.end())
        {
            for (auto cand = obj.begin(); cand != obj.end(); ++cand)
            {
                if (Node::iequals(cand->first, key))
                {
                    it = cand;
                    break;
                }
            }
        }
        if (it == obj.end())
        {
            obj.emplace(key, Node::Array{std::move(value)});
            return;
        }
        if (!it->second.is_array())
        {
            Node::Array arr;
            arr.push_back(std::move(it->second));
            it->second = std::move(arr);
        }
        it->second.as_array().push_back(std::move(value));
    }

    static void append_anonymous_item(Node::Object& obj, Node value)
    {
        append_repeated(obj, "$items", std::move(value));
    }

    std::vector<Token> tokens_;
    size_t pos_ = 0;
    Metadata pending_meta_;
    std::vector<std::string> path_stack_;
    std::map<std::string, Metadata> metadata_;

    std::string current_path() const
    {
        return path_stack_.empty() ? std::string{} : path_stack_.back();
    }

    std::string child_metadata_key(const std::string& key) const
    {
        return current_path().empty() ? key : (current_path() + "." + key);
    }

    std::string block_metadata_key(const std::string& key, const Node* label) const
    {
        std::string out = child_metadata_key(key);
        if (label)
        {
            out += ":";
            out += label->as_string();
        }
        return out;
    }

    void store_metadata(const std::string& key, Metadata meta)
    {
        if (!options_.preserve_comments)
            return;
        if (meta.leading_blank_lines == 0 && meta.leading_comments.empty())
            return;
        metadata_[key] = std::move(meta);
    }

    ParseOptions options_;
};

inline bool needs_quotes(const std::string& s)
{
    if (s.empty()) return true;
    for (char c : s)
        if (!(std::isalnum((unsigned char)c) || c == '_' || c == '-' || c == '.' || c == '\\' || c == '/'))
            return true;
    return Node::iequals(s, "true") || Node::iequals(s, "false") || Node::iequals(s, "null");
}

inline std::string quote(const std::string& s)
{
    std::string out = "\"";
    for (char c : s)
    {
        switch (c)
        {
        case '\n': out += "\\n"; break;
        case '\t': out += "\\t"; break;
        case '\r': out += "\\r"; break;
        case '\\': out += "\\\\"; break;
        case '"': out += "\\\""; break;
        default: out.push_back(c); break;
        }
    }
    out.push_back('"');
    return out;
}

inline bool can_emit_inline_value(const Node& n);
inline std::string value_to_def(const Node& n, const WriteOptions* options, const std::string& path);
inline std::string object_to_inline_def(const Node::Object& obj, const WriteOptions* options, const std::string& path);

inline bool can_emit_inline_array(const Node::Array& arr)
{
    if (arr.size() > 8)
        return false;
    for (const Node& item : arr)
        if (!can_emit_inline_value(item))
            return false;
    return true;
}

inline bool can_emit_inline_object(const Node::Object& obj)
{
    if (obj.empty() || obj.size() > 4)
        return false;
    if (obj.find("$name") != obj.end())
        return false;
    for (const auto& kv : obj)
        if (!can_emit_inline_value(kv.second))
            return false;
    return true;
}

inline bool can_emit_inline_value(const Node& n)
{
    if (n.is_null() || n.is_bool() || n.is_number() || n.is_string())
        return true;
    if (n.is_array())
    {
        const auto& arr = n.as_array();
        if (arr.size() > 8)
            return false;
        for (const Node& item : arr)
            if (!(item.is_null() || item.is_bool() || item.is_number() || item.is_string()))
                return false;
        return true;
    }
    if (n.is_object())
        return can_emit_inline_object(n.as_object());
    return false;
}

inline AssignStyle assign_style_for_path(const WriteOptions* options, const std::string& path)
{
    if (!options)
        return AssignStyle::Equals;
    auto unlabeled = [](const std::string& p) {
        std::string out;
        bool skipping = false;
        for (char c : p)
        {
            if (c == ':') { skipping = true; continue; }
            if (skipping && c == '.') { skipping = false; out.push_back(c); continue; }
            if (!skipping) out.push_back(c);
        }
        return out;
    };
    const std::string plain_path = unlabeled(path);
    for (const WriteRule& rule : options->rules)
        if (rule.has_assign_style && (Node::iequals(rule.path, path) ||
                                      Node::iequals(rule.path, plain_path)))
            return rule.assign_style;
    return options->assign_style;
}

inline const char* assignment_separator(const WriteOptions* options, const std::string& path)
{
    return assign_style_for_path(options, path) == AssignStyle::Space ? " " : " = ";
}

inline std::string object_to_inline_def(const Node::Object& obj, const WriteOptions* options, const std::string& path)
{
    std::string out = "{ ";
    bool first = true;
    for (const auto& kv : obj)
    {
        if (!first) out += ", ";
        first = false;
        const std::string child_path = path.empty() ? kv.first : (path + "." + kv.first);
        out += kv.first;
        out += assignment_separator(options, child_path);
        out += value_to_def(kv.second, options, child_path);
    }
    out += " }";
    return out;
}

inline std::string object_to_inline_def(const Node::Object& obj)
{
    WriteOptions options;
    return object_to_inline_def(obj, &options, {});
}

inline std::string array_to_def(const Node::Array& arr, const WriteOptions* options = nullptr, const std::string& path = {})
{
    std::string out = "[";
    for (size_t i = 0; i < arr.size(); ++i)
    {
        if (i) out += ", ";
        out += value_to_def(arr[i], options, path + "[]");
    }
    out += "]";
    return out;
}

inline std::string value_to_def(const Node& n, const WriteOptions* options, const std::string& path)
{
    switch (n.type())
    {
    case Node::Type::Null: return "null";
    case Node::Type::Bool: return n.as_bool() ? "true" : "false";
    case Node::Type::Int: return std::to_string(n.as_int());
    case Node::Type::Double:
    {
        std::ostringstream ss; ss << n.as_double(); return ss.str();
    }
    case Node::Type::String:
    {
        const std::string s = n.as_string();
        return needs_quotes(s) ? quote(s) : s;
    }
    case Node::Type::Array: return array_to_def(n.as_array(), options, path);
    case Node::Type::Object:
        return can_emit_inline_object(n.as_object()) ? object_to_inline_def(n.as_object(), options, path) : "{...}";
    }
    return "null";
}

inline std::string value_to_def(const Node& n)
{
    return value_to_def(n, nullptr, {});
}

inline WriteStyle style_for_path(const WriteOptions* options, const std::string& path)
{
    if (!options)
        return WriteStyle::Auto;
    auto unlabeled = [](const std::string& p) {
        std::string out;
        bool skipping = false;
        for (char c : p)
        {
            if (c == ':') { skipping = true; continue; }
            if (skipping && c == '.') { skipping = false; out.push_back(c); continue; }
            if (!skipping) out.push_back(c);
        }
        return out;
    };
    const std::string plain_path = unlabeled(path);
    for (const WriteRule& rule : options->rules)
        if (Node::iequals(rule.path, path) || Node::iequals(rule.path, plain_path))
            return rule.style;
    return WriteStyle::Auto;
}

inline std::string join_path(const std::string& parent, const std::string& child)
{
    return parent.empty() ? child : (parent + "." + child);
}

inline void emit_object_def(std::ostream& os, const Node::Object& obj, int indent,
                            const WriteOptions* options = nullptr,
                            const std::string& path = {},
                            const std::map<std::string, Metadata>* metadata = nullptr)
{
    const std::string pad(size_t(indent), ' ');
    auto emit_metadata = [&](const std::string& key) {
        if (!metadata) return;
        auto it = metadata->find(key);
        if (it == metadata->end()) return;
        for (int i = 0; i < it->second.leading_blank_lines; ++i)
            os << "\n";
        for (const std::string& comment : it->second.leading_comments)
            os << pad << "// " << comment << "\n";
    };
    for (const auto& kv : obj)
    {
        if (kv.first == "$name") continue;
        const std::string child_path = join_path(path, kv.first);
        const WriteStyle style = style_for_path(options, child_path);
        const Node& n = kv.second;
        bool emitted_block = false;
        if (n.is_array())
        {
            const auto& arr = n.as_array();
            if (!arr.empty() && arr[0].is_object())
            {
                if (style != WriteStyle::Block && (style == WriteStyle::Inline || can_emit_inline_array(arr)))
                {
                    emit_metadata(child_path);
                    os << pad << kv.first
                       << assignment_separator(options, child_path)
                       << array_to_def(n.as_array(), options, child_path) << "\n";
                    emitted_block = true;
                }
                else
                {
                    for (const Node& child : arr)
                    {
                        std::string block_path = child_path;
                        if (child.contains("$name"))
                            block_path += ":" + child["$name"].as_string();
                        emit_metadata(block_path);
                        os << pad << kv.first;
                        if (child.contains("$name"))
                            os << " " << value_to_def(child["$name"], options, child_path + ".$name");
                        os << "\n" << pad << "BEGIN\n";
                        emit_object_def(os, child.as_object(), indent + 2, options, block_path, metadata);
                        os << pad << "END\n";
                    }
                    emitted_block = true;
                }
            }
        }
        if (!emitted_block && n.is_object())
        {
            if (style != WriteStyle::Block && (style == WriteStyle::Inline || can_emit_inline_object(n.as_object())))
            {
                emit_metadata(child_path);
                os << pad << kv.first
                   << assignment_separator(options, child_path)
                   << value_to_def(n, options, child_path) << "\n";
            }
            else
            {
                emit_metadata(child_path);
                os << pad << kv.first << "\n" << pad << "BEGIN\n";
                emit_object_def(os, n.as_object(), indent + 2, options, child_path, metadata);
                os << pad << "END\n";
            }
        }
        else if (!emitted_block)
        {
            emit_metadata(child_path);
            os << pad << kv.first
               << assignment_separator(options, child_path)
               << value_to_def(n, options, child_path) << "\n";
        }
    }
}

} // namespace detail

class Document
{
public:
    Node& root() { return root_; }
    const Node& root() const { return root_; }
    std::map<std::string, Metadata>& metadata() { return metadata_; }
    const std::map<std::string, Metadata>& metadata() const { return metadata_; }

    Metadata& metadata_for(const std::string& key) { return metadata_[key]; }
    const Metadata* find_metadata(const std::string& key) const
    {
        auto it = metadata_.find(key);
        return it == metadata_.end() ? nullptr : &it->second;
    }

    static Document parse(const std::string& text, ParseOptions options = {})
    {
        detail::Lexer lex(text);
        detail::Parser parser(lex.lex(), options);
        Document doc;
        doc.root_ = parser.parse_document();
        doc.metadata_ = parser.metadata();
        return doc;
    }

    static Document parse_file(const std::string& path, ParseOptions options = {})
    {
        std::ifstream f(path);
        if (!f)
            throw std::runtime_error("unable to open DEF file: " + path);
        std::ostringstream ss;
        ss << f.rdbuf();
        return parse(ss.str(), options);
    }

    std::string to_def(const WriteOptions& options) const
    {
        std::ostringstream ss;
        detail::emit_object_def(ss, root_.as_object(), 0, &options, {}, &metadata_);
        return ss.str();
    }

    std::string to_def() const
    {
        WriteOptions options;
        return to_def(options);
    }

    bool write_file(const std::string& path, const WriteOptions& options) const
    {
        std::ofstream f(path);
        if (!f) return false;
        f << to_def(options);
        return f.good();
    }

    bool write_file(const std::string& path) const
    {
        WriteOptions options;
        return write_file(path, options);
    }

private:
    Node root_{Node::Object{}};
    std::map<std::string, Metadata> metadata_;
};

inline Document parse(const std::string& text, ParseOptions options = {}) { return Document::parse(text, options); }
inline Document parse_file(const std::string& path, ParseOptions options = {}) { return Document::parse_file(path, options); }

} // namespace defdoc
