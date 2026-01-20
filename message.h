#ifndef MESSAGE_H
#define MESSAGE_H

#include <QUuid>
#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Message {
public:
    Message();
    Message(const QUuid &id,
            const QUuid &senderId,
            const QUuid &receiverId,
            const QString &content,
            const QDateTime &timestamp);

    QUuid getId() const { return m_id; }
    QUuid getSenderId() const { return m_senderId; }

    // ВАЖНО: DataManager использует getReceiverId() [file:4]
    QUuid getReceiverId() const { return m_receiverId; }

    // На случай если где-то использовано другое имя:
    QUuid getRecipientId() const { return m_receiverId; }

    QString getContent() const { return m_content; }
    QDateTime getTimestamp() const { return m_timestamp; }

    void setId(const QUuid &id) { m_id = id; }
    void setSenderId(const QUuid &id) { m_senderId = id; }
    void setReceiverId(const QUuid &id) { m_receiverId = id; }
    void setContent(const QString &content) { m_content = content; }
    void setTimestamp(const QDateTime &ts) { m_timestamp = ts; }

    QJsonObject toJson() const;
    static Message fromJson(const QJsonObject &json);

private:
    QUuid m_id;
    QUuid m_senderId;
    QUuid m_receiverId;
    QString m_content;
    QDateTime m_timestamp;
};

#endif // MESSAGE_H
