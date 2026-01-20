#ifndef REVIEW_H
#define REVIEW_H

#include <QUuid>
#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Review {
public:
    Review(const QUuid &id = QUuid::createUuid(),
           const QUuid &clientId = QUuid(),
           const QUuid &serviceId = QUuid(),
           double rating = 0.0,
           const QString &comment = "");

    QUuid getId() const { return m_id; }
    QUuid getClientId() const { return m_clientId; }
    QUuid getServiceId() const { return m_serviceId; }
    double getRating() const { return m_rating; }
    QString getComment() const { return m_comment; }
    QDateTime getCreatedAt() const { return m_createdAt; }

    QJsonObject toJson() const;
    static Review fromJson(const QJsonObject &json);

private:
    QUuid m_id;
    QUuid m_clientId;
    QUuid m_serviceId;
    double m_rating;
    QString m_comment;
    QDateTime m_createdAt;
};

#endif // REVIEW_H
