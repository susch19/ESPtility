#include "string.hpp"
#include <Print.h>

namespace esptility
{
    string::string(const string &other) : _length(other.length())
    {
        _myStrings.reserve(other._myStrings.size());
        for (auto &&block : other._myStrings)
        {
            auto blockData = new char[STRINGBLOCKSIZE];

            // Serial.printf("Mem Copy Ctor string We:%d, Them:%d, Ram: %d\n", _myStrings.size(), other._myStrings.size(), ESP.getFreeHeap());
            memcpy(blockData, block, STRINGBLOCKSIZE);

            _myStrings.push_back(blockData);
        }
    }
    string::string(const char *text) : string(text, strlen(text)) {}

    string::string(const String &str) : string(str.c_str(), str.length())
    {
    }

    string::string(const char *text, size_t size)
    {
        _length = size;

        size_t blockCount = (size + STRINGBLOCKSIZE - 1) / STRINGBLOCKSIZE;
        _myStrings.reserve(blockCount);
        for (size_t i = 0; i < blockCount; i++)
        {
            auto blockData = new char[STRINGBLOCKSIZE];

            // Serial.printf("Mem Copy Ctor: %d\n", ESP.getFreeHeap());
            memcpy(blockData, text + i * STRINGBLOCKSIZE, std::min(size, (size_t)STRINGBLOCKSIZE));

            _myStrings.emplace_back(blockData);
            size -= STRINGBLOCKSIZE;
        }
    }
    size_t string::copyFrom(const void *src, size_t offset, size_t length)
    {
        auto blockIndex = offset / STRINGBLOCKSIZE;
        size_t index = offset % STRINGBLOCKSIZE;
        // gdb_do_break();
        auto targetBlockIndex = (offset + length) / STRINGBLOCKSIZE;
        size_t copied = 0;
        for (size_t i = blockIndex; i <= targetBlockIndex; i++)
        {
            auto toCopy = std::min(STRINGBLOCKSIZE - index, length);
            Serial.printf("Copy %d bytes into %d index in %d block. Capacity: %d, Length: %d\n", toCopy, index, i, blockLength(), size());
            // gdb_do_break();
            memcpy(&(_myStrings[i][index]), src, toCopy);
            // gdb_do_break();
            index = 0;
            length -= toCopy;
            src = src + toCopy;
            copied += toCopy;
        }
        Serial.printf("Copied %d bytes\n", copied);
        return copied;
    }

    size_t string::copyTo(void *dest, size_t offset, size_t length) const
    {
        auto blockIndex = offset / STRINGBLOCKSIZE;
        size_t index = offset % STRINGBLOCKSIZE;

        auto targetBlockIndex = (offset + length) / STRINGBLOCKSIZE;
        size_t copied = 0;
        for (size_t i = blockIndex; i <= targetBlockIndex; i++)
        {
            auto toCopy = std::min(STRINGBLOCKSIZE - index, length);
            memcpy(dest, &(_myStrings[i][index]), toCopy);
            index = 0;
            length -= toCopy;
            dest = dest + toCopy;
            copied += toCopy;
        }
        return copied;
    }

    size_t string::copy(string &dest, size_t sourceOffset, size_t destOffset, size_t length) const
    {
        auto blockIndex = sourceOffset / STRINGBLOCKSIZE;
        size_t index = sourceOffset % STRINGBLOCKSIZE;

        auto targetBlockIndex = (sourceOffset + length) / STRINGBLOCKSIZE;
        size_t copied = destOffset;
        for (size_t i = blockIndex; i <= targetBlockIndex; i++)
        {
            auto toCopy = std::min(STRINGBLOCKSIZE - index, length);
            for (size_t inner = 0; inner < toCopy; inner++)
            {
                dest[copied++] = _myStrings[i][index + inner];
            }

            index = 0;
            length -= toCopy;
        }
        return copied - destOffset;
    }

    size_t string::length() const
    {
        return _length;
    }
    size_t string::size() const
    {
        return length();
    }

    void string::append(const string &other)
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

    void string::append(const char *text, size_t size)
    {
        reserve(size + length());
        for (size_t i = 0; i < size; i++)
        {
            push_back_unsafe(text[i]);
        }
    }

    void string::reserve(size_t size)
    {
        size_t blockIndex = size / STRINGBLOCKSIZE;
        for (size_t i = blockLength(); i <= blockIndex; i++)
        {
            _myStrings.emplace_back(new char[STRINGBLOCKSIZE]);
        }
    }

    void string::push_back(const char c)
    {
        reserve(length() + 1);
        push_back_unsafe(c);
    }

    void string::replace(const char c, size_t index)
    {
        size_t blockIndex = index / STRINGBLOCKSIZE;

        index = index % STRINGBLOCKSIZE;
        _myStrings[blockIndex][index] = c;
    }

    void string::resize(size_t size)
    {
        reserve(size);
        _length = size;
    }
    size_t string::blockLength() const
    {
        return _myStrings.size();
    }

    int string::compareBlocks(size_t blockIndex, const string &other) const
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
    size_t string::printTo(Print &p) const
    {
        auto size = length();
        for (auto &&str : _myStrings)
        {
            auto toWrite = std::min(size, (size_t)STRINGBLOCKSIZE);
            size -= toWrite;
            p.write(str, toWrite);
        }
        return size;
    }

    string &string::operator=(const string &other)
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

    string string::operator+(const string &other) const
    {
        string newString = *this;
        newString.append(other);
        return newString;
    }
    string &string::operator+=(const string &other)
    {
        this->append(other);
        return *this;
    }
    string &string::operator+=(const char other)
    {
        this->push_back(other);
        return *this;
    }
    bool string::operator==(const string &other) const
    {
        return other.length() == length() && (*this <=> other) == 0;
    }

    bool string::operator!=(const string &other) const
    {
        return other.length() != length() || (*this <=> other) != 0;
    }

    string &string::operator<<(const string &s)
    {
        return (*this += s);
    }
    char &string::operator[](const size_t index) const
    {
        string newString = *this;

        size_t blockIndex = index / STRINGBLOCKSIZE;
        size_t arrIndex = index % STRINGBLOCKSIZE;
        return _myStrings[blockIndex][arrIndex];
    }
    char &string::operator[](const int index) const
    {
        string newString = *this;

        size_t blockIndex = index / STRINGBLOCKSIZE;
        size_t arrIndex = index % STRINGBLOCKSIZE;
        return _myStrings[blockIndex][arrIndex];
    }

    int string::operator<=>(const string &other) const
    {
        for (size_t i = 0; i < std::max(blockLength(), other.blockLength()); i++)
        {
            auto comparison = compareBlocks(i, other);
            if (comparison != 0)
                return comparison;
        }
        return 0;
    }
    void string::push_back_unsafe(const char c)
    {
        auto targetLength = length();
        size_t blockIndex = targetLength / STRINGBLOCKSIZE;

        size_t index = targetLength % STRINGBLOCKSIZE;
        _myStrings[blockIndex][index] = c;
        _length++;
    }

    void string::clear()
    {
        for (auto &&i : _myStrings)
        {
            delete[] i;
        }
        _myStrings.resize(0);
    }
}