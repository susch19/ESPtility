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

    string::string(const String  &str) : string(str.c_str(), str.length()){

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

}