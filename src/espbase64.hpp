#ifndef _SMARTHOME_BASE64_HPP_
#define _SMARTHOME_BASE64_HPP_

#include <string>
#include "string.hpp"

namespace base64
{

    static inline bool is_base64(unsigned char c)
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    static const char *chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    inline std::string encodeStd(unsigned char const *bytes_to_encode,
                                 unsigned int in_len)
    {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--)
        {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                                  ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                                  ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] =
                ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] =
                ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (j = 0; (j < i + 1); j++)
                ret += chars[char_array_4[j]];
            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    inline std::string encodeStd(const std::string &str64)
    {
        return encodeStd((unsigned char *)str64.c_str(), str64.length());
    }

    static const int B64index[256] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 62, 62, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,
            0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 63,
            0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

    inline const std::string decodeStd(const void *data, const size_t &len, size_t &resLen)
    {
        resLen = 0;
        if (len == 0)
            return "";

        unsigned char *p = (unsigned char *)data;
        size_t j = 0,
               pad1 = len % 4 || p[len - 1] == '=',
               pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
        const size_t last = (len - pad1) / 4 << 2;

        std::string result(last / 4 * 3 + pad1 + pad2, '\0');

        unsigned char *str = (unsigned char *)&result[0];

        for (size_t i = 0; i < last; i += 4)
        {
            int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
            str[j++] = n >> 16;
            str[j++] = n >> 8 & 0xFF;
            str[j++] = n & 0xFF;
        }
        if (pad1)
        {
            int n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
            str[j++] = n >> 16;
            if (pad2)
            {
                n |= B64index[p[last + 2]] << 6;
                str[j++] = n >> 8 & 0xFF;
            }
        }
        resLen = j;
        return result;
    }

    inline std::string decodeStd(const std::string &str64)
    {
        size_t tmp;
        return decodeStd(str64.c_str(), str64.length(), tmp);
    }

    inline std::string decodeStd(const std::string &str64, size_t &resLen)
    {
        return decodeStd(str64.c_str(), str64.length(), resLen);
    }

    inline esptility::string encode(const esptility::string &toEncode,
                                    unsigned int in_len)
    {
        esptility::string ret;
        int i = 0;
        int j = 0;
        size_t offset = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--)
        {
            char_array_3[i++] = toEncode[offset++];
            if (i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                                  ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                                  ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                {
                    ret += chars[char_array_4[i]];
                }
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] =
                ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] =
                ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (j = 0; (j < i + 1); j++)
                ret += chars[char_array_4[j]];
            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    inline esptility::string encode(const esptility::string &str64)
    {
        return encode(str64, str64.length());
    }

    inline const esptility::string decode(const esptility::string &data, const size_t &len, size_t &resLen)
    {
        resLen = 0;
        if (len == 0)
            return "";

        auto p = data;
        size_t j = 0,
               pad1 = len % 4 || p[len - 1] == '=',
               pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
        const size_t last = (len - pad1) / 4 << 2;

        esptility::string result;
        //result.reserve(last / 4 * 3 + pad1 + pad2);

        for (size_t i = 0; i < last; i += 4)
        {
            int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
            result += n >> 16;
            result+= n >> 8 & 0xFF;
            result+= n & 0xFF;
        }
        if (pad1)
        {
            int n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
            result+= n >> 16;
            if (pad2)
            {
                n |= B64index[p[last + 2]] << 6;
                result+= n >> 8 & 0xFF;
            }
        }
        resLen = result.length();
        return result;
    }

    inline esptility::string decode(const esptility::string &str64)
    {
        size_t tmp;
        return decode(str64, str64.length(), tmp);
    }

    inline esptility::string decode(const esptility::string &str64, size_t &resLen)
    {
        return decode(str64, str64.length(), resLen);
    }
} // namespace base64
#endif
