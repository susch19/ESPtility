#ifndef _SERIALIZER_HPP_
#define _SERIALIZER_HPP_

#include <string>
#include <type_traits>
#include <cstring>
#include <string.hpp>

struct SerializeHelper
{
  template <class T>
  static void deserialize(T *dest, const esptility::string &str, int &offset);
  template <class T>
  static void serialize(const T *source, esptility::string &str, int &offset);
};

template <class T, typename = void>
struct InternalSerializer
{
  static void deserialize(T *dest, const esptility::string &str, int &offset)
  {
    static_assert(!std::is_same<T, T>::value, "Type can't be deserialized!");
  }
  static void serialize(const T *source, esptility::string &str, int &offset)
  {
    static_assert(!std::is_same<T, T>::value, "Type can't be serialized!");
  }
};

template <class T>
struct InternalSerializer<
    T, typename std::enable_if<std::is_trivially_copyable<T>::value>::type>
{
  static void deserialize(T *dest, const esptility::string &str, int &offset)
  {
    offset += sizeof(T) * str.copyTo(dest, offset, 1);
  }
  static void serialize(const T *source, esptility::string &str, int &offset)
  {
    uint32_t neededSpace = static_cast<size_t>(offset + sizeof(T));
    if (str.size() < neededSpace)
      str.resize(neededSpace);
  
    offset += sizeof(T) * str.copyFrom(source, offset, 1);
  }
};
template <>
struct InternalSerializer<esptility::string, void>
{
  static void deserialize(esptility::string *dest, const esptility::string &str,
                          int &offset)
  {
    uint16_t length;
    SerializeHelper::deserialize(&length, str, offset);
    dest->resize(length);
    offset += str.copy(*dest, offset, 0, length);
  }
  static void serialize(const esptility::string *source, esptility::string &str, int &offset)
  {
    uint16_t length = source->size();
    uint32_t neededSpace = offset + length + sizeof(length);
    if (str.size() < neededSpace)
      str.resize(neededSpace);
    SerializeHelper::serialize(&length, str, offset);
    offset += source->copy(str, 0, offset, length);
  }
};

template <class T>
void SerializeHelper::deserialize(T *dest, const esptility::string &str,
                                  int &offset)
{
  InternalSerializer<T>::deserialize(dest, str, offset);
}
template <class T>
void SerializeHelper::serialize(const T *source, esptility::string &str, int &offset)
{
  InternalSerializer<T>::serialize(source, str, offset);
}

#endif