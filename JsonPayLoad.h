#ifndef JSONPAYLOAD_H
#define JSONPAYLOAD_H
#pragma once
#include <vector>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

namespace JsonPayload {

inline std::vector<char> toBytes(const QJsonObject &obj) {
    QByteArray json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return std::vector<char>(json.begin(), json.end());
}
inline QJsonObject fromBytes(const std::vector<char> &data) {
    QByteArray json(data.data(), static_cast<int>(data.size()));
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return QJsonObject(); // payload خالی/خراب -> آبجکت خالی برمی‌گردد، طرف صدازننده باید چک کند
    }
    return doc.object();
}
inline QJsonObject fromBytes(const QByteArray &data) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        return QJsonObject();
    }
    return doc.object();
}
inline QByteArray toQByteArray(const QJsonObject &obj) {
    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

}
#endif // JSONPAYLOAD_H