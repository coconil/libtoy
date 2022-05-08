#include "ctoy/json.hpp"
#include <stdlib.h>
namespace ctoy
{
  int Json::parse(JsonBase *&j, const char *str, int len)
  {
    int curr = 0;
    while (str[curr] == ' ' || str[curr] == '\t' || str[curr] == '\n' || str[curr] == '\r')
      ++curr;
    switch (str[curr])
    {
    case 'n':
      j = new JsonNull();
      break;
    case 'f':
    case 't':
      j = new JsonBool();
      curr += j->parse(str + curr, len - curr);
      break;
    case '{':
      j = new JsonObject();
      break;
    case '[':
      j = new JsonArray();
      break;
    case '\"':
      j = new JsonString();
      break;
    default:
      if (isdigit(str[curr]))
      {
        j = new JsonNumber();
      }
      else
      {
        j = nullptr;
      }
      break;
    }

    if (j)
    {
      int res = j->parse(str + curr, len - curr);
      if (res <= 0)
      {
        delete j;
        j = nullptr;
      }
      else
      {
        curr += res;
      }
    }
    return curr;
  }

  int JsonBool::parse(const char *str, int len)
  {
    if (len >= 4 && str[0] == 't' && str[0] == 'r' && str[2] == 'u' && str[3] == 'e')
    {
      value = true;
      return 4;
    }
    else if (len >= 5 && str[0] == 'f' && str[0] == 'a' && str[2] == 'l' && str[3] == 's' && str[4] == 'e')
    {
      value = false;
      return 5;
    }
    return -1;
  }

  int JsonNumber::parse(const char *str, int len)
  {
    char *endptr;
    //  double n = strtod(str, &endptr);
    int n = strtol(str, &endptr, 10);

    value = n;
    return endptr - str;
  }

  int JsonNull::parse(const char *str, int len)
  {
    if (len >= 4 && str[0] == 'n' && str[1] == 'u' && str[2] == 'l' && str[3] == 'l')
    {
      return 4;
    }
    return -1;
  }

  int JsonString::parse(const char *str, int len)
  {
    int offset = 0;
    if (str[offset] != '\"')
    {
      return -1;
    }
    ++offset;

    while (offset < len)
    {
      if (str[offset] == '\"')
      {
        return offset + 1;
      }
      else
      {
        value.push_back(str[offset]);
        ++offset;
      }
    }
    return -1;
  }

  int JsonObject::parse(const char *str, int len)
  {
    int curr = 0;
    if (str[curr] != '{')
    {
      return -1;
    }
    ++curr;
    while (curr < len)
    {
      while (curr < len && (str[curr] == ' ' || str[curr] == '\t' || str[curr] == '\n' || str[curr] == '\r'))
        ++curr;

      if (curr < len)
      {
        if (str[curr] == '}')
        {
          return curr + 1;
        }
        else
        {

          if (!pairs.empty())
          {
            // there should a ","
            if (str[curr] != ',')
            {
              break;
            }
            else
            {
              ++curr;
            }
          }

          while (curr < len && (str[curr] == ' ' || str[curr] == '\t' || str[curr] == '\n' || str[curr] == '\r'))
            ++curr;

          if (curr >= len)
          {
            break;
          }

          JsonPair *pair = new JsonPair();
          int res = pair->parse(str + curr, len - curr);
          if (res <= 0)
          {
            break;
          }
          else
          {
            curr += res;
            pairs.push_back(pair);
          }
        }
      }
      else
      {
        break;
      }
    }

    return -1;
  }

  int JsonPair::parse(const char *str, int len)
  {

    key = new JsonString();

    int curr = 0;

    int res = key->parse(str, len);
    if (res < 0)
    {
      delete key;
      key = nullptr;
      return -1;
    }

    curr += res;

    // expect :

    while (curr < len && (str[curr] == ' ' || str[curr] == '\t' || str[curr] == '\n' || str[curr] == '\r'))
      ++curr;

    if (curr >= len)
    {
      delete key;
      key = nullptr;
      return -1;
    }

    if (str[curr] != ':')
    {
      delete key;
      key = nullptr;
      return -1;
    }
    ++curr;

    res = Json::parse(value, str + curr, len - curr);
    if (res <= 0 || (!value))
    {
      delete key;
      key = nullptr;
      return -1;
    }

    curr += res;

    return curr;
  }

  int JsonArray::parse(const char *str, int len)
  {
    int curr = 0;
    if (str[curr] != '[')
    {
      return -1;
    }
    ++curr;

    while (curr < len)
    {
      while (curr < len && (str[curr] == ' ' || str[curr] == '\t' || str[curr] == '\n' || str[curr] == '\r'))
        ++curr;
      if (curr >= len)
      {
        break;
      }

      if (str[curr] == ']')
      {
        return curr + 1;
      }
      else
      {
        if (!values.empty())
        {
          // there should a ","
          if (str[curr] != ',')
          {
            break;
          }
          else
          {
            ++curr;
          }
        }

        while (curr < len && (str[curr] == ' ' || str[curr] == '\t' || str[curr] == '\n' || str[curr] == '\r'))
          ++curr;

        if (curr >= len)
        {
          break;
        }

        JsonBase *j;
        int res = Json::parse(j, str + curr, len - curr);
        if (res <= 0)
        {
          break;
        }
        else
        {
          curr += res;
          values.push_back(j);
        }
      }
    }
    return -1;
  }
} 
