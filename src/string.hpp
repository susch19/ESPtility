#pragma once

#include <list>
#include <memory>
#include <cstring>
#include <algorithm>
#include <Printable.h>
#include <Arduino.h>

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
        string(string &&other) = default;
        string(const string &other);

        string(const char *text);

        string(const char *text, size_t size);

        string(const String &str);

        size_t length() const
        {
            return _length;
        }
        size_t size() const
        {
            return length();
        }

        size_t printTo(Print &p) const override;

        void append(const char *text, size_t size)
        {
            reserve(size + length());
            for (size_t i = 0; i < size; i++)
            {
                push_back_unsafe(text[i]);
            }
        }
        void append(const string &other)
        {
            int size = other.length();
            reserve(length() + size);
            for (auto &&block : other._myStrings)
            {
                auto ptr = block;
                for (size_t i = 0; i < STRINGBLOCKSIZE; i++)
                {

                    if (--size < 0)
                        return;
                    push_back_unsafe(ptr[i]);
                }
            }
        }
        void resize(size_t size)
        {
            reserve(size);
            _length = size;
        }

        void reserve(size_t size)
        {
            size_t startBlockIndex = length() / STRINGBLOCKSIZE;
            size_t blockIndex = size / STRINGBLOCKSIZE;
            for (size_t i = startBlockIndex; i < blockIndex; i++)
            {
                _myStrings.emplace_back(new char[STRINGBLOCKSIZE]);
            }
        }

        void push_back(const char c)
        {
            resize(length() + 1);
            size_t blockIndex = length() / STRINGBLOCKSIZE;

            size_t index = length() % STRINGBLOCKSIZE;
            _myStrings[blockIndex][index] = c;
        }

        string operator+(const string &other) const
        {
            string newString = *this;
            newString.append(other);
            return newString;
        }
        string &operator+=(const string &other)
        {
            this->append(other);
            return *this;
        }

        string &operator=(const string &other)
        {
            clear();

            _myStrings.reserve(other._myStrings.size());
            _length = other.length();
            for (auto &&block : other._myStrings)
            {
                auto blockData = new char[STRINGBLOCKSIZE];

                // Serial.printf("Mem Copy Ctor string We:%d, Them:%d, Ram: %d\n", _myStrings.size(), other._myStrings.size(), ESP.getFreeHeap());
                memcpy(blockData, block, STRINGBLOCKSIZE);

                _myStrings.push_back(blockData);
            }
            return *this;
        }

        bool operator==(const string &other) const
        {
            return other.length() == length() && (*this <=> other) == 0;
        }

        bool operator!=(const string &other) const
        {
            return other.length() != length() || (*this <=> other) != 0;
        }

        int operator<=>(const string &other) const
        {
            for (size_t i = 0; i < std::max(blockLength(), other.blockLength()); i++)
            {
                auto comparison = compareBlocks(i, other);
                if (comparison != 0)
                    return comparison;
            }
            return 0;
        }
        int compareBlocks(size_t blockIndex, const string &other) const
        {
            bool hasBlockA = blockIndex < blockLength();
            bool hasBlockB = blockIndex < other.blockLength();

            auto comparison = hasBlockA - hasBlockB;

            if (comparison != 0 || !hasBlockA)
                return comparison;

            auto blockA = this->_myStrings[blockIndex];
            auto blockALength = length() / STRINGBLOCKSIZE == blockIndex ? length() % STRINGBLOCKSIZE : STRINGBLOCKSIZE;
            auto blockB = other._myStrings[blockIndex];
            auto blockBLength = other.length() / STRINGBLOCKSIZE == blockIndex ? other.length() % STRINGBLOCKSIZE : STRINGBLOCKSIZE;
            for (size_t i = 0; i < std::min(blockALength, blockBLength); i++)
            {
                auto innerComparison = blockA[i] - blockB[i];
                if (innerComparison != 0)
                    return innerComparison;
            }
            return sgn(blockALength - blockBLength);
        }
        char &operator[](const size_t index)
        {
            string newString = *this;

            size_t blockIndex = index / STRINGBLOCKSIZE;
            size_t arrIndex = index % STRINGBLOCKSIZE;
            return _myStrings[blockIndex][arrIndex];
        }

        string &operator<<(const string &s){
            return (*this += s);
        }

        template<class T>
        string &operator<<(const T &v){
            String fFormatted(v);
            return (*this += fFormatted.c_str());
        }

        virtual ~string()
        {
            clear();
        }

        size_t blockLength() const
        {
            return _myStrings.size();
        }

    private:
        void push_back_unsafe(const char c)
        {
            auto targetLength = length();
            size_t blockIndex = targetLength / STRINGBLOCKSIZE;

            size_t index = targetLength % STRINGBLOCKSIZE;
            _myStrings[blockIndex][index] = c;
            _length++;
        }

        void clear(){
            for (auto &&i : _myStrings)
            {
                delete[] i;
            }
            _myStrings.resize(0);
        }

        //"toStdstring", "toString", c_str, (<<)
    };
}