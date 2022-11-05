#pragma once

#include <list>
#include <memory>
#include <cstring>
#include <algorithm>
#include <Printable.h>
#include <Arduino.h>
#include <GDBStub.h>

#ifndef STRINGBLOCKSIZE
#define STRINGBLOCKSIZE 32
#endif
template <typename T>
int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

class Print;
namespace esptility
{
    class string : public Printable
    {
    private:
        std::vector<char *> _myStrings;
        size_t _length;

    public:
        string() : _length(0), _myStrings() {}
        string(string &&other) = default;
        string(const string &other);

        string(const char *text);

        string(const char *text, size_t size);

        string(const String &str);

        size_t length() const;
        size_t size() const;

        size_t printTo(Print &p) const override;

        void append(const char *text, size_t size);
        void append(const string &other);

        void resize(size_t size);

        void reserve(size_t size);

        void push_back(const char c);
        void replace(const char c, size_t index);

        string operator+(const string &other) const;
        string &operator+=(const string &other);
        string &operator+=(const char other);

        string &operator=(const string &other);

        bool operator==(const string &other) const;
        bool operator!=(const string &other) const;
        int operator<=>(const string &other) const;
        int compareBlocks(size_t blockIndex, const string &other) const;

        char &operator[](const size_t index) const;
        char &operator[](const int index) const;

        string &operator<<(const string &s);

        template <class T>
        string &operator<<(const T &v)
        {
            String fFormatted(v);
            return (*this += fFormatted.c_str());
        }

        virtual ~string()
        {
            clear();
        }

        size_t copy(string &dest, size_t sourceOffset, size_t destOffset, size_t length) const;

        size_t copyTo(void *dest, size_t offset, size_t length) const;

        template <class T>
        size_t copyTo(T *destinations, size_t offset, size_t count) const
        {
            return copyTo(static_cast<void *>(destinations), offset, sizeof(T) * count) / sizeof(T);
        }

        size_t copyFrom(const void *src, size_t offset, size_t length);

        template <class T>
        size_t copyFrom(const T *sources, size_t offset, size_t count)
        {
            return copyFrom(static_cast<const void *>(sources), offset, sizeof(T) * count) / sizeof(T);
        }

    private:
        void push_back_unsafe(const char c);

        void clear();

        size_t blockLength() const;
        //"toStdstring", "toString", c_str, (<<)
    };
}