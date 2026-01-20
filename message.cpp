#include "message.h"

Message::Message()
    : m_id(QUuid::createUuid())
    , m_timestamp(QDateTime::currentDateTime())
{
}

Message::Message(const QUuid &id,
                 const QUuid &senderId,
                 const QUuid &receiverId,
                 const QString &content,
                 const QDateTime &timestamp)
    : m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_senderId(senderId)
    , m_receiverId(receiverId)
    , m_content(content)
    , m_timestamp(timestamp.isValid() ? timestamp : QDateTime::currentDateTime())
{
}

QJsonObject Message::toJson() const
{
    QJsonObject json;
    json["id"] = m_id.toString(QUuid::WithoutBraces);
    json["senderId"] = m_senderId.toString(QUuid::WithoutBraces);
    json["receiverId"] = m_receiverId.toString(QUuid::WithoutBraces);
    json["content"] = m_content;
    json["timestamp"] = m_timestamp.toString(Qt::ISODate);
    return json;
}

Message Message::fromJson(const QJsonObject &json)
{
    Message msg(
        QUuid(json.value("id").toString()),
        QUuid(json.value("senderId").toString()),
        QUuid(json.value("receiverId").toString()),
        json.value("content").toString(),
        QDateTime::fromString(json.value("timestamp").toString(), Qt::ISODate)
        );

    if (msg.m_id.isNull())
        msg.m_id = QUuid::createUuid();

    return msg;
}
