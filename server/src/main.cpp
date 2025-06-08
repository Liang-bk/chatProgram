#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "header/cserver.h"
#include <cctype>
unsigned char decToHex(unsigned char x) {
    return x > 9 ? x + 55 : x + 48;
}
int hexToDec(char ch) {
    if ('0' <= ch && ch <= '9') return ch - '0';
    if ('A' <= ch && ch <= 'F') return ch - 'A' + 10;
    if ('a' <= ch && ch <= 'f') return ch - 'a' + 10;
    return -1; // 非法字符
}
// http url编码, 转换空格和其他非ascii字符
std::string UrlEncode(const std::string& str) {
    std::string encoded = "";
    size_t length = str.length();
    for (int i = 0; i < length; i++) {
        unsigned char c = str[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else if (c == ' ') {
            encoded += '+';
        } else {
            encoded += '%';
            encoded += decToHex((unsigned char)c >> 4);
            encoded += decToHex((unsigned char)c & 0x0F);
        }
    }
    return encoded;
}

// http url解码
std::string UrlDecode(const std::string& str) {
    std::string decoded = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+') {
            decoded += ' ';
        } else if (str[i] == '%') {
            assert(i + 2 < length);
            char high = hexToDec(str[++i]);
            char low = hexToDec(str[++i]);
            // unsigned char c = HexToDec(str[i + 1]);
            // c = (c << 4) + HexToDec(str[i + 2]);
            decoded += (char)(high * 16 + low);
            // i += 2;
        } else {
            decoded += str[i];
        }
    }
    return decoded;
}
int main()
{
    try
    {
        unsigned short port = static_cast<unsigned short>(8000);
        net::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_num) {
            if (error) {
                return;
            }
            ioc.stop();

        });
        std::make_shared<CServer>(ioc, port)->start();
        ioc.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}