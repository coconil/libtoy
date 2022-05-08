#pragma once
//#include "list.hpp"
#include <string>
#include <vector>
namespace ctoy {
class JsonBase;
enum class JsonType { JSON_NUM, JSON_NULL, JSON_STR, JSON_OBJ, JSON_BOOL, JSON_ARRAY };
class Json {
public:
  static int parse(JsonBase *&j, const char *str, int len);
  JsonType get_type() const;
};

class JsonBase {
public:
  JsonType type;
  JsonBase(JsonType t) : type(t) {
  }
  virtual ~JsonBase(){};
  virtual int parse(const char *str, int len) = 0;
};

class JsonNull final : public JsonBase {
public:
  JsonNull() : JsonBase(JsonType::JSON_NULL) {
  }
  int parse(const char *str, int len) override;
};

class JsonNumber final : public JsonBase {
public:
  JsonNumber() : JsonBase(JsonType::JSON_NUM) {
  }
  ~JsonNumber() {
  }
  int parse(const char *str, int len) override;
  bool operator==(int x) {
    return x == value;
  }
  bool operator==(JsonNumber &n) {
    return n == value;
  }

private:
  int value;
  // double value = 0;
};

class JsonString final : public JsonBase {
public:
  JsonString() : JsonBase(JsonType::JSON_STR) {
  }
  ~JsonString() {
  }
  int parse(const char *str, int len) override;

private:
  std::string value;
};

class JsonArray final : public JsonBase {
public:
  JsonArray() : JsonBase(JsonType::JSON_ARRAY) {
  }
  ~JsonArray() {
  }
  int parse(const char *str, int len) override;
  int length() {
    return values.size();
  }
  JsonBase *operator[](int n) {
    return values[n];
  }

private:
  std::vector<JsonBase *> values;
};

class JsonPair {
public:
  JsonPair() : key(nullptr), value(nullptr) {
  }
  int parse(const char *str, int len);
  JsonString *key;
  JsonBase *value;
};

class JsonObject : public JsonBase {
public:
  JsonObject() : JsonBase(JsonType::JSON_OBJ) {
  }
  ~JsonObject() {
  }
  int parse(const char *str, int len) override;

private:
  std::vector<JsonPair *> pairs;
};

class JsonBool final : public JsonBase {
public:
  JsonBool() : JsonBase(JsonType::JSON_BOOL) {
  }
  ~JsonBool() {
  }
  int parse(const char *str, int len) override;

private:
  bool value = false;
};

} 
