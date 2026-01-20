#ifndef REQUEST_H
#define REQUEST_H

#include <QUuid>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

class Request
{
public:
    enum class Status {
        Pending = 0,
        Accepted = 1,
        InProgress = 2,
        Completed = 3,
        Cancelled = 4
    };

    Request(const QUuid &id = QUuid(),
            const QUuid &serviceId = QUuid(),
            const QUuid &clientId = QUuid(),
            const QUuid &providerId = QUuid());

    // Getters
    QUuid getId() const { return m_id; }
    QUuid getServiceId() const { return m_serviceId; }
    QUuid getClientId() const { return m_clientId; }
    QUuid getProviderId() const { return m_providerId; }

    QString getDescription() const { return m_description; }
    Status getStatus() const { return m_status; }

    // Для QML/DataManager (преобразование статуса в int)
    int getStatusIndex() const { return static_cast<int>(m_status); }

    QDateTime getCreatedAt() const { return m_createdAt; }
    QDateTime getCompletedAt() const { return m_completedAt; }

    QStringList getComments() const { return m_comments; }

    // Setters
    void setDescription(const QString &description);
    void updateStatus(Status newStatus);
    void addComment(const QString &comment);

    // Для DataManager
    void setStatusFromInt(int index) {
        if (index < 0) index = 0;
        if (index > 4) index = 4;
        updateStatus(static_cast<Status>(index));
    }

    // Helper methods
    QString getStatusString() const;

    // Info methods
    QString getInfo() const;
    QString getFullInfo() const;

    // JSON serialization
    QJsonObject toJson() const;
    static Request fromJson(const QJsonObject &json);

private:
    static Status statusFromString(const QString &s);
    static Status statusFromInt(int v);

    QUuid m_id;
    QUuid m_serviceId;
    QUuid m_clientId;
    QUuid m_providerId;

    QString m_description;
    Status m_status = Status::Pending;

    QDateTime m_createdAt;
    QDateTime m_completedAt;

    QStringList m_comments;
};

#endif // REQUEST_H
