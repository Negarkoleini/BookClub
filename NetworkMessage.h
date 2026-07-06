#pragma once
#include <vector>
#include "CommandType.h"

// قالب‌بندی پیام‌های شبکه‌ای بین کلاینت و سرور
// ساختار باینری: [commandType (4 بایت)][dataLength (4 بایت)][payload]
class NetworkMessage {
private:
    CommandType commandType;
    int dataLength;
    std::vector<char> payload;

public:
    NetworkMessage(CommandType commandType,const std::vector<char>& payload);
    CommandType getCommandType() const;
    int getDataLength() const;
    const std::vector<char>& getPayload() const;

    std::vector<char> wrap() const;                      // تبدیل پیام به بایت‌های خام برای ارسال روی سوکت
    static NetworkMessage unwrap(const std::vector<char>& rawData); // بازسازی پیام از بایت‌های دریافتی

    static const int HEADER_SIZE = static_cast<int>(2 * sizeof(int));
};
