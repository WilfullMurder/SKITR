#ifndef SKITR_VALUE_H
#define SKITR_VALUE_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <stdexcept>

#include <skitr/skitr_export.h>

namespace skitter {
    namespace core {

        /**
         * @enum Type
         * @brief Represents the underlying type stored in a Value.
         *
         * This enum enumerates all supported value kinds. New kinds may be added
         * to represent additional payloads (e.g. Binary, DateTime).
         */
        enum class Type {
            Null, ///< Represents a null or empty value.
            Bool, ///< Represents a boolean value (true or false).
            Int, ///< Represents a 64-bit integer value.
            Double, ///< Represents a double-precision floating-point value.
            String, ///< Represents a string value.
            Array, ///< Represents an ordered list of values (similar to JSON arrays).
            Object, ///< Represents a collection of key-value pairs (similar to JSON objects).
            Binary, ///< Represents binary data as a vector of bytes.
            DateTime ///< Represents a date/time value (system clock time point).
        };

        /**
         * @class Value
         * @brief A variant type used to represent dynamically-typed values.
         *
         * Value stores one of several kinds (null, bool, int, double, string,
         * array, object, binary, datetime) and provides convenience accessors,
         * mutators and JSON-like serialization.
         *
         * The implementation uses std::variant for storage. Arrays and objects
         * are reference-counted via shared_ptr to allow copy-on-write semantics.
         */
        class SKITR_EXPORT Value {
        public:
            using array_t = std::vector<Value>; ///< Alias for array type
            using object_t = std::map<std::string, Value>; ///< Alias for object type
            using array_ptr = std::shared_ptr<array_t>; ///< Shared pointer for array storage
            using object_ptr = std::shared_ptr<object_t>; ///< Shared pointer for object storage
            using binary_t = std::vector<uint8_t>; ///< Alias for binary data type
            using datetime_t = std::chrono::system_clock::time_point; ///< Alias for date/time type
            using storage_t = std::variant<std::monostate, bool, int64_t, double, std::string, array_ptr, object_ptr, binary_t, datetime_t>; ///< The underlying storage variant type

            /**
             * @brief Default constructs a Null Value.
             */
            Value() noexcept : type_(Type::Null), storage_(std::monostate{}) {}

            /**
             * @brief Default constructs a Null Value.
             */
            Value() noexcept : type_(Type::Null), storage_(std::monostate{}) {}

            /**
             * @brief Construct a boolean Value.
             * @param b boolean payload
             */
            Value(bool b) : type_(Type::Bool), storage_(b) {}

            /**
             * @brief Construct an integer Value (int64_t).
             * @param i integer payload
             */
            Value(int64_t i) : type_(Type::Int), storage_(i) {}

            /**
             * @brief Construct from int by promoting to int64_t.
             * @param i integer payload
             */
            Value(int i) : Value(static_cast<int64_t>(i)) {}

            /**
             * @brief Construct a double Value.
             * @param d floating point payload
             */
            Value(double d) : type_(Type::Double), storage_(d) {}

            /**
             * @brief Construct a string Value from const reference.
             * @param s string payload
             */
            Value(const std::string &s) : type_(Type::String), storage_(s) {}

            /**
             * @brief Construct a string Value by moving.
             * @param s string payload (moved)
             */
            Value(std::string &&s) : type_(Type::String), storage_(std::move(s)) {}

            /**
             * @brief Construct a string Value from C-string.
             * @param s null-terminated C string
             */
            Value(const char *s) : type_(Type::String), storage_(std::string(s)) {}

            /**
             * @brief Construct an array Value from const reference.
             * @param a array payload
             */
            Value(const array_t &a) : type_(Type::Array), storage_(std::make_shared<array_t>(a)) {}

            /**
             * @brief Construct an array Value by moving.
             * @param a array payload (moved)
             */
            Value(array_t &&a) : type_(Type::Array), storage_(std::make_shared<array_t>(std::move(a))) {}

            /**
             * @brief Construct an object Value from const reference.
             * @param o object payload
             */
            Value(const object_t &o) : type_(Type::Object), storage_(std::make_shared<object_t>(o)) {}

            /**
             * @brief Construct an object Value by moving.
             * @param o object payload (moved)
             */
            Value(object_t &&o) : type_(Type::Object), storage_(std::make_shared<object_t>(std::move(o))) {}

            /**
             * @brief Construct a binary Value from const reference.
             * @param b binary payload
             */
            Value(const binary_t &b) : type_(Type::Binary), storage_(b) {}

            /**
             * @brief Construct a binary Value by moving.
             * @param b binary payload (moved)
             */
            Value(binary_t &&b) : type_(Type::Binary), storage_(std::move(b)) {}

            /**
             * @brief Construct a datetime Value.
             * @param dt time_point payload
             */
            Value(const datetime_t &dt) : type_(Type::DateTime), storage_(dt) {}

            /**
             * @brief Return a human-readable name for a Type.
             * @param t type to stringify
             * @return name of the type
             */
            static std::string typeName(Type t) noexcept {
                switch (t) {
                    case Type::Null: return "Null";
                    case Type::Bool: return "Bool";
                    case Type::Int: return "Int";
                    case Type::Double: return "Double";
                    case Type::String: return "String";
                    case Type::Array: return "Array";
                    case Type::Object: return "Object";
                    case Type::Binary: return "Binary";
                    case Type::DateTime: return "DateTime";
                    default: return "Unknown";
                }
            }

            /**
 * @brief Get the current stored Type.
 * @return the Type of this Value
 */
            Type type() const noexcept;
            /**
             * @brief Checks if this Value is Null.
             * @return true if Null
             */
            bool isNull() const noexcept;
            /**
             * @brief Checks if this Value is a Bool.
             * @return true if Bool
             */
            bool isBool() const noexcept;
            /**
             * @brief Checks if this Value is an Int.
             * @return true if Int
             */
            bool isInt() const noexcept;
            /**
             * @brief Checks if this Value is a Double.
             * @return true if Double
             */
            bool isDouble() const noexcept;
            /**
             * @brief Checks if this Value is a String.
             * @return true if String
             */
            bool isString() const noexcept;
            /**
             * @brief Checks if this Value is an Array.
             * @return true if Array
             */
            bool isArray() const noexcept;
            /**
             * @brief Checks if this Value is an Object.
             * @return true if Object
             */
            bool isObject() const noexcept;
            /**
             * @brief Checks if this Value is Binary.
             * @return true if Binary
             */
            bool isBinary() const noexcept;
            /**
             * @brief Checks if this Value is a DateTime.
             * @return true if DateTime
             */
            bool isDateTime() const noexcept;

            /**
             * @brief Retrieve boolean payload.
             * @return stored boolean
             * @throws std::bad_variant_access if the stored type is not Bool
             */
            bool asBool() const;
            /**
             * @brief Retrieve integer payload.
             * @return stored int64_t
             * @throws std::bad_variant_access if the stored type is not Int
             */
            int64_t asInt() const;
            /**
             * @brief Retrieve double payload.
             * @return stored double
             * @throws std::bad_variant_access if the stored type is not Double
             */
            double asDouble() const;
            /**
             * @brief Retrieve const reference to string payload.
             * @return const reference to stored std::string
             * @throws std::bad_variant_access if the stored type is not String
             */
            const std::string &asString() const;
            /**
             * @brief Get mutable reference to array payload.
             * @return reference to vector of Values
             * @throws std::runtime_error if the Value is not an Array
             * @note Calling this may trigger copy-on-write semantics.
             */
            array_t &asArray();
            /**
             * @brief Get mutable reference to object payload.
             * @return reference to map of Values
             * @throws std::runtime_error if the Value is not an Object
             * @note Calling this may trigger copy-on-write semantics.
             */
            object_t &asObject();
            /**
             * @brief Get const reference to array payload.
             * @return const reference to array
             * @throws std::bad_variant_access if not Array
             */
            const array_t &asArray() const;
            /**
             * @brief Get const reference to object payload.
             * @return const reference to object
             * @throws std::bad_variant_access if not Object
             */
            const object_t &asObject() const;
            /**
             * @brief Get mutable reference to binary payload.
             * @return reference to binary blob
             * @throws std::runtime_error if this Value is not Binary
             * @note May promote Null to Binary when used as non-const accessor.
             */
            binary_t &asBinary();
            /**
             * @brief Get const reference to binary payload.
             * @return const reference to binary blob
             * @throws std::bad_variant_access if not Binary
             */
            const binary_t &asBinary() const;
            /**
             * @brief Get mutable reference to datetime payload.
             * @return reference to stored time_point
             * @throws std::runtime_error if this Value is not DateTime
             * @note May promote Null to DateTime when used as non-const accessor.
             */
            datetime_t &asDateTime();

            /**
             * @brief Set this Value to Null.
             */
            void setNull() noexcept;
            /**
             * @brief Set boolean payload.
             * @param b boolean value
             */
            void setBool(bool b);
            /**
             * @brief Set integer payload.
             * @param i int64_t value
             */
            void setInt(int64_t i);
            /**
             * @brief Set double payload.
             * @param d double value
             */
            void setDouble(double d);
            /**
             * @brief Set string payload from const reference.
             * @param s string value
             */
            void setString(const std::string &s);
            /**
             * @brief Set array payload from const reference.
             * @param a array value
             */
            void setArray(const array_t &a);
            /**
             * @brief Set object payload from const reference.
             * @param o object value
             */
            void setObject(const object_t &o);
            /**
             * @brief Set binary payload.
             * @param binary binary blob
             */
            void setBinary(const binary_t &binary);
            /**
             * @brief Set datetime payload.
             * @param dt time_point value
             */
            void setDateTime(const datetime_t &dt);

            /**
             * @brief Append a Value to the array payload.
             * @param v value to append (moved or copied)
             * @throws std::runtime_error if this Value is not an Array
             * @note This will ensure unique array storage before modifying (COW).
             */
            void pushBack(Value v);

            /**
           * @brief Access or create an entry in an object by key.
           * @param key object key
           * @return reference to the mapped Value
           * @throws std::runtime_error if this Value is not an Object (unless Null,
           *         in which case it is promoted to Object)
           *
           * If the current Value is Null, it will be promoted to an Object and a new
           * mapping inserted. If an existing object is shared, copy-on-write is applied.
           */
            Value &operator[](const std::string &key);

            /**
             * @brief Produce a JSON-like textual representation of the Value.
             * @return string containing the serialized representation
             *
             * Strings are escaped, binary is encoded (e.g. base64) and datetimes
             * formatted in ISO8601 (UTC) according to implementation.
             */
            std::string toString() const;

        private:
            /**
             * @brief Helper used by toString to write the representation into a stream.
             * @param os output string stream to append to
             */
            void writeToStream(std::ostringstream &os) const;
            /**
             * @brief Escape a string for JSON-like serialization.
             * @param s input string
             * @return escaped string (with surrounding quotes)
             */
            static std::string escapeString(const std::string &s);

            /**
             * @brief Ensure the internal array is uniquely owned before mutating.
             *
             * If the array is shared by multiple Values, it will be cloned so that
             * modifications do not affect other copies (copy-on-write).
             */
            void ensureUniqueArray();
            /**
             * @brief Ensure the internal object is uniquely owned before mutating.
             *
             * If the object is shared by multiple Values, it will be cloned so that
             * modifications do not affect other copies (copy-on-write).
             */
            void ensureUniqueObject();

            Type type_; ///< The current type of the stored value
            storage_t storage_; ///< The underlying storage for the value, using std::variant
    };
    } // namespace core
} // namespace skitter


#endif //SKITR_VALUE_H
