#include <skitr/core/value.h>
#include <stdexcept>
#include <utility>
#include <memory>
#include <type_traits>
#include <string>
#include <vector>
#include <map>
#include <variant>

namespace skitter {
    namespace core {
        std::string Value::typeName(Type t) noexcept {
            switch (t) {
                case Type::Null: return "Null";
                case Type::Bool: return "Bool";
                case Type::Int: return "Int";
                case Type::Double: return "Double";
                case Type::String: return "String";
                case Type::Array: return "Array";
                case Type::Object: return "Object";
                default: return "Unknown";
            }
        }

        Value::Value() noexcept: type_(Type::Null), storage_(std::monostate{}) {}
        Value::Value(bool b): type_(Type::Bool), storage_(b) {}
        Value::Value(int64_t i): type_(Type::Int), storage_(i) {}
        Value::Value(double d): type_(Type::Double), storage_(d) {}
        Value::Value(const std::string& s): type_(Type::String), storage_(s) {}
        Value::Value(std::string&& s): type_(Type::String), storage_(std::move(s)) {}
        Value::Value(const char* s): type_(Type::String), storage_(std::string(s)) {}
        Value::Value(const std::vector<Value>& arr): type_(Type::Array), storage_(std::make_shared<array_t>(arr)) {}
        Value::Value(std::vector<Value>&& arr): type_(Type::Array), storage_(std::make_shared<array_t>(std::move(arr))) {}
        Value::Value(const std::map<std::string, Value>& obj): type_(Type::Object), storage_(std::make_shared<object_t>(obj)) {}
        Value::Value(std::map<std::string, Value>&& obj): type_(Type::Object), storage_(std::make_shared<object_t>(std::move(obj))) {}

        Value::Value(const Value& other): type_(other.type_){
            switch (other.type_) {
                case Type::Null:
                    storage_ = std::monostate{};
                    break;
                case Type::Bool:
                    storage_ = std::get<bool>(other.storage_);
                    break;
                case Type::Int:
                    storage_ = std::get<int64_t>(other.storage_);
                    break;
                case Type::Double:
                    storage_ = std::get<double>(other.storage_);
                    break;
                case Type::String:
                    storage_ = std::get<std::string>(other.storage_);
                    break;
                case Type::Array: {
                    auto p = std::get<array_ptr>(other.storage_);
                    storage_ = std::make_shared<array_t>(p ? *p : array_t{});
                    break;
                }
                case Type::Object: {
                    auto p = std::get<object_ptr>(other.storage_);
                    storage_ = std::make_shared<object_t>(p ? *p : object_t{});
                    break;
                }
            }
        }
        Value::Value(Value&& other) noexcept: type_(other.type_), storage_(std::move(other.storage_)) {
            other.type_ = Type::Null;
            other.storage_ = std::monostate{};
        }

        Value& Value::operator=(const Value& other) {
            if (this == &other) return *this;
            Value tmp(other);
            *this = std::move(tmp);
            return *this;
        }

        Type Value::type() const noexcept { return type_; }
        bool Value::isNull() const noexcept { return type_ == Type::Null; }
        bool Value::isBool() const noexcept { return type_ == Type::Bool; }
        bool Value::isInt() const noexcept { return type_ == Type::Int; }
        bool Value::isDouble() const noexcept { return type_ == Type::Double; }
        bool Value::isString() const noexcept { return type_ == Type::String; }
        bool Value::isArray() const noexcept { return type_ == Type::Array; }
        bool Value::isObject() const noexcept { return type_ == Type::Object; }
        bool Value::isBinary() const noexcept { return type_ == Type::Binary; }
        bool Value::isDateTime() const noexcept { return type_ == Type::DateTime; }

        bool Value::asBool() const {
            if (!isBool()) throw std::runtime_error("Value::asBool: type is " + typeName(type_));
            return std::get<bool>(storage_);
        }

        int64_t Value::asInt() const {
            if (!isInt()) throw std::runtime_error("Value::asInt: type is " + typeName(type_));
            return std::get<int64_t>(storage_);
        }

        double Value::asDouble() const {
            if (!isDouble()) throw std::runtime_error("Value::asDouble: type is " + typeName(type_));
            return std::get<double>(storage_);
        }

        const std::string& Value::asString() const {
            if (!isString()) throw std::runtime_error("Value::asString: type is " + typeName(type_));
            return std::get<std::string>(storage_);
        }

        std::vector<Value>& Value::asArray() {
            if (isNull()) {
                type_ = Type::Array;
                storage_ = std::make_shared<array_t>();
            } else if (!isArray()) {
                throw std::runtime_error("Value::asArray: type is " + typeName(type_));
            }
            ensureUniqueArray();
            return *std::get<array_ptr>(storage_);
        }

        std::map<std::string, Value>& Value::asObject() {
            if (isNull()) {
                type_ = Type::Object;
                storage_ = std::make_shared<object_t>();
            } else if (!isObject()) {
                throw std::runtime_error("Value::asObject: type is " + typeName(type_));
            }
            ensureUniqueObject();
            return *std::get<object_ptr>(storage_);
        }

        const Value::array_t& Value::asArray() const {
            if (!isArray()) throw std::runtime_error("Value::asArray: type is " + typeName(type_));
            const auto &ptr = std::get<array_ptr>(storage_);
            static const array_t empty_array{};
            return ptr ? *ptr : empty_array;
        }

        const Value::object_t& Value::asObject() const {
            if (!isObject()) throw std::runtime_error("Value::asObject: type is " + typeName(type_));
            const auto &ptr = std::get<object_ptr>(storage_);
            static const object_t empty_object{};
            return ptr ? *ptr : empty_object;
        }

        Value::binary_t &Value::asBinary() {
            if(!isBinary()) throw std::runtime_error("Value::asBinary: type is " + typeName(type_));
            return std::get<binary_t>(storage_);
        }

        const Value::binary_t &Value::asBinary() const {
            if(!isBinary()) throw std::runtime_error("Value::asBinary: type is " + typeName(type_));
            return std::get<binary_t>(storage_);
        }

        std::chrono::system_clock::time_point &Value::asDateTime() {
            if(!isDateTime()) throw std::runtime_error("Value::asDateTime: type is " + typeName(type_));
            return std::get<datetime_t>(storage_);
        }

        void Value::setNull() noexcept {
            storage_ = std::monostate{};
            type_ = Type::Null;
        }

        void Value::setBool(bool b) {
            storage_ = b;
            type_ = Type::Bool;
        }

        void Value::setInt(int64_t i) {
            storage_ = i;
            type_ = Type::Int;
        }

        void Value::setDouble(double d) {
            storage_ = d;
            type_ = Type::Double;
        }

        void Value::setString(const std::string &s) {
            storage_ = s;
            type_ = Type::String;
        }

        void Value::setArray(const Value::array_t &a) {
            storage_ = std::make_shared<array_t>(a);
            type_ = Type::Array;
        }

        void Value::setObject(const Value::object_t &o) {
            storage_ = std::make_shared<object_t>(o);
            type_ = Type::Object;
        }

        void Value::setBinary(const Value::binary_t &binary) {
            storage_ = binary;
            type_ = Type::Binary;
        }

        void Value::setDateTime(const std::chrono::system_clock::time_point &dt) {
            storage_ = dt;
            type_ = Type::DateTime;
        }

        void Value::pushBack(Value v) {
            if(!isArray()) setArray(array_t{});
            asArray().push_back(std::move(v));
        }

        Value &Value::operator[](const std::string &key) {
            if (!isObject()) setObject(object_t{});
            return asObject()[key];
        }

        std::string Value::toString() const {
            std::ostringstream os;
            writeToStream(os);
            return os.str();
        }

        void Value::writeToStream(std::ostringstream &os) const {
            switch (type_) {
                case Type::Null:
                    os << "null";
                    break;
                case Type::Bool:
                    os << (std::get<bool>(storage_) ? "true" : "false");
                    break;
                case Type::Int:
                    os << std::get<int64_t>(storage_);
                    break;
                case Type::Double:
                    os << std::get<double>(storage_);
                    break;
                case Type::String:
                    os << '"' << escapeString(std::get<std::string>(storage_)) << '"';
                    break;
                case Type::Array: {
                    os << '[';
                    const auto &ptr = *std::get<array_ptr>(storage_);
                    for (size_t i = 0; i < ptr.size(); ++i) {
                        if (i > 0) os << ',';
                        os << ptr[i].toString();
                    }
                    os << ']';
                    break;
                }
                case Type::Object: {
                    os << '{';
                    const auto &ptr = *std::get<object_ptr>(storage_);
                    bool first = true;
                    for (const auto &kv: ptr) {
                        if (!first) os << ',';
                        first = false;
                        os << '"' << escapeString(kv.first) << "\":" << kv.second.toString();
                    }
                    os << '}';
                    break;
                }
                case Type::Binary: {
                    os << "\"<binary:" << std::get<binary_t>(storage_).size() << " bytes>\"";
                    break;
                }
                case Type::DateTime: {
                    auto tp = std::get<datetime_t>(storage_);
                    std::time_t t = std::chrono::system_clock::to_time_t(tp);
                    os << '"' << std::asctime(std::localtime(&t)) << "";
                    break;
                }
            }
        }

        std::string Value::escapeString(const std::string &s) {
            std::string out;
            out.reserve(s.size() + 4);
            for (char c: s) {
                switch (c) {
                    case '"': out += "\\\""; break;
                    case '\\': out += "\\\\"; break;
                    case '\n': out += "\\n"; break;
                    case '\r': out += "\\r"; break;
                    case '\t': out += "\\t"; break;
                    default: out += c; break;
                }
            }
            return out;
        }

        void Value::ensureUniqueArray() {
            if(!isArray()){
                throw std::runtime_error("Value is not an array");
            }
            auto &ptr = std::get<array_ptr>(storage_);
            if(!ptr) {
                ptr = std::make_shared<array_t>();
            }else if(!ptr.unique()) {
                ptr = std::make_shared<array_t>(*ptr);
            }
        }

        void Value::ensureUniqueObject() {
            if(!isObject()){
                throw std::runtime_error("Value is not an object");
            }
            auto &ptr = std::get<object_ptr>(storage_);
            if(!ptr) {
                ptr = std::make_shared<object_t>();
            }else if(!ptr.unique()) {
                ptr = std::make_shared<object_t>(*ptr);
            }
        }
    } // namespace core
} // namespace skitter