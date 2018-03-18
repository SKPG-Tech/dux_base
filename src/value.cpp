#include "value.h"

#include <cassert>

namespace dux {

Value::Value(Type type) : type_(type) {
  switch (type_) {
    case Type::NONE:
      return;
    case Type::BOOLEAN:
      bool_value_ = false;
      return;
    case Type::DOUBLE:
      double_value_ = 0.0;
      return;
    case Type::STRING:
      new (&string_value_) std::string();
      return;
    case Type::DICTIONARY:
      new (&dictionary_) DictionaryStorage();
      return;
    case Type::LIST:
      new (&list_) ListStorage();
      return;
  }
}

Value::Value(bool in_bool) : type_(Type::BOOLEAN), bool_value_(in_bool) {}

Value::Value(int32_t in_int) : type_(Type::DOUBLE), double_value_(in_int) {}

Value::Value(int64_t in_int) : type_(Type::DOUBLE), double_value_(in_int) {}

Value::Value(double in_double)
    : type_(Type::DOUBLE), double_value_(in_double) {}

Value::Value(std::string const& in_string)
    : type_(Type::STRING), string_value_(in_string) {}

Value::Value(char const* in_string)
    : type_(Type::STRING), string_value_(in_string) {}

Value::Value(Value&& other) {
  type_ = other.type_;

  switch (type_) {
    case Type::NONE:
      return;
    case Type::BOOLEAN:
      bool_value_ = other.bool_value_;
      return;
    case Type::DOUBLE:
      double_value_ = other.double_value_;
      return;
    case Type::STRING:
      new (&string_value_) std::string(std::move(other.string_value_));
      return;
    case Type::DICTIONARY:
      new (&dictionary_) DictionaryStorage(std::move(other.dictionary_));
      return;
    case Type::LIST:
      new (&list_) ListStorage(std::move(other.list_));
      return;
  }
}

Value::~Value() {}

bool Value::GetBool() const {
  assert(IsBool());
  return bool_value_;
}

double Value::GetDouble() const {
  if (IsDouble())
    return double_value_;
  assert(false);
  return 0.0;
}

const std::string& Value::GetString() const {
  assert(IsString());
  return string_value_;
}

Value::ListStorage& Value::GetList() {
  assert(IsList());
  return list_;
}

const Value::ListStorage& Value::GetList() const {
  assert(IsList());
  return list_;
}

Value::DictionaryStorage& Value::GetDictionary() {
  assert(IsDictionary());
  return dictionary_;
}

const Value::DictionaryStorage& Value::GetDictionary() const {
  assert(IsDictionary());
  return dictionary_;
}

Value* Value::FindKeyOfType(std::string const& key, Type type) {
  return const_cast<Value*>(
      static_cast<const Value*>(this)->FindKeyOfType(key, type));
}

const Value* Value::FindKeyOfType(std::string const& key, Type type) const {
  auto it = dictionary_.find(key);
  if (it == dictionary_.end())
    return nullptr;
  const Value* result = it->second.get();
  if (result->GetType() != type)
    return nullptr;
  return result;
}

void Value::SetKey(std::string const& key, std::unique_ptr<Value> value) {
  assert(IsDictionary());
  dictionary_[key] = std::move(value);
}

bool Value::RemoveKey(std::string const& key) {
  assert(IsDictionary());
  return dictionary_.erase(key) > 0;
}

}  // namespace dux