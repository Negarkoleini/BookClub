#include "NetworkMessage.h"

#include <cstring>
#include <stdexcept>

NetworkMessage::NetworkMessage(CommandType commandType,
                               const std::vector<char>& payload)
    : commandType(commandType),
    dataLength(static_cast<int>(payload.size())),
    payload(payload)
{
}

CommandType NetworkMessage::getCommandType() const
{
    return commandType;
}

int NetworkMessage::getDataLength() const
{
    return dataLength;
}

const std::vector<char>& NetworkMessage::getPayload() const
{
    return payload;
}

std::vector<char> NetworkMessage::wrap() const
{
    std::vector<char> raw;
    raw.resize(HEADER_SIZE + payload.size());

    int cmd = static_cast<int>(commandType);

    std::memcpy(raw.data(), &cmd, sizeof(int));
    std::memcpy(raw.data() + sizeof(int), &dataLength, sizeof(int));

    if (!payload.empty())
    {
        std::memcpy(raw.data() + HEADER_SIZE,
                    payload.data(),
                    payload.size());
    }

    return raw;
}

NetworkMessage NetworkMessage::unwrap(const std::vector<char>& rawData)
{
    if (rawData.size() < static_cast<size_t>(HEADER_SIZE))
    {
        throw std::runtime_error(
            "NetworkMessage::unwrap - Header is incomplete");
    }

    int cmd = 0;
    int length = 0;

    std::memcpy(&cmd,
                rawData.data(),
                sizeof(int));

    std::memcpy(&length,
                rawData.data() + sizeof(int),
                sizeof(int));

    if (length < 0 ||
        static_cast<size_t>(HEADER_SIZE + length) > rawData.size())
    {
        throw std::runtime_error(
            "NetworkMessage::unwrap - Invalid payload length");
    }

    std::vector<char> payloadData(rawData.begin() + HEADER_SIZE, rawData.begin() + HEADER_SIZE + length);

    return NetworkMessage(
        static_cast<CommandType>(cmd),
        payloadData);
}