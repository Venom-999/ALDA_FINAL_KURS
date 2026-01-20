#include "review.h"

Review::Review(const QUuid &id, const QUuid &clientId, const QUuid &serviceId, double rating, const QString &comment)
    : m_id(id.isNull() ? QUuid::createUuid() : id),
    m_clientId(clientId),
    m_serviceId(serviceId),
    m_rating(rating),
    m_comment(comment),
    m_createdAt(QDateTime::currentDateTime()) {}

QJsonObject Review::toJson() const {
    QJsonObject json;
    json["id"] = m_id.toString(QUuid::WithoutBraces);
    json["clientId"] = m_clientId.toString(QUuid::WithoutBraces);
    json["serviceId"] = m_serviceId.toString(QUuid::WithoutBraces);
    json["rating"] = m_rating;
    json["comment"] = m_comment;
    json["createdAt"] = m_createdAt.toString(Qt::ISODate);
    return json;
}

Review Review::fromJson(const QJsonObject &json) {
    Review r(QUuid(json["id"].toString()),
             QUuid(json["clientId"].toString()),
             QUuid(json["serviceId"].toString()),
             json["rating"].toDouble(),
             json["comment"].toString());
    r.m_createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
    return r;
}
