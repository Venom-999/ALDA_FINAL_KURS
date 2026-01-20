#include "profile.h"

#include <QJsonArray>

Profile::Profile(const QUuid& profileId,
                 const QUuid& ownerUserId,
                 const QString& name,
                 const QString& description)
    : m_profileId(profileId.isNull() ? QUuid::createUuid() : profileId)
    , m_ownerUserId(ownerUserId)
    , m_name(name)
    , m_description(description)
{
    m_createdAt = QDateTime::currentDateTime();
}

void Profile::setOwnerUserId(const QUuid& id)
{
    m_ownerUserId = id;
}

void Profile::setName(const QString& name)
{
    m_name = name;
}

void Profile::setDescription(const QString& description)
{
    m_description = description;
}

void Profile::setAvatarPath(const QString& path)
{
    m_avatarPath = path;
}

void Profile::setContactEmail(const QString& email)
{
    m_contactEmail = email;
}

void Profile::setContactPhone(const QString& phone)
{
    m_contactPhone = phone;
}

void Profile::setVerified(bool verified)
{
    m_isVerified = verified;
}

void Profile::addRating(float r)
{
    if (r < 0.0f || r > 5.0f)
        return;

    m_rating = (m_rating * m_reviewCount + r) / float(m_reviewCount + 1);
    m_reviewCount++;
}

QJsonObject Profile::toJson() const
{
    QJsonObject j;
    j["profileId"] = m_profileId.toString(QUuid::WithoutBraces);
    j["ownerUserId"] = m_ownerUserId.toString(QUuid::WithoutBraces);

    j["name"] = m_name;
    j["description"] = m_description;
    j["avatarPath"] = m_avatarPath;

    j["contactEmail"] = m_contactEmail;
    j["contactPhone"] = m_contactPhone;

    j["rating"] = m_rating;
    j["reviewCount"] = m_reviewCount;

    j["isVerified"] = m_isVerified;
    j["createdAt"] = m_createdAt.toString(Qt::ISODate);
    return j;
}

Profile Profile::fromJson(const QJsonObject& j)
{
    Profile p(
        QUuid(j.value("profileId").toString()),
        QUuid(j.value("ownerUserId").toString()),
        j.value("name").toString(),
        j.value("description").toString()
        );

    p.m_avatarPath = j.value("avatarPath").toString();
    p.m_contactEmail = j.value("contactEmail").toString();
    p.m_contactPhone = j.value("contactPhone").toString();

    p.m_rating = float(j.value("rating").toDouble(0.0));
    p.m_reviewCount = j.value("reviewCount").toInt(0);

    p.m_isVerified = j.value("isVerified").toBool(false);
    p.m_createdAt = QDateTime::fromString(j.value("createdAt").toString(), Qt::ISODate);
    if (!p.m_createdAt.isValid())
        p.m_createdAt = QDateTime::currentDateTime();

    if (p.m_profileId.isNull())
        p.m_profileId = QUuid::createUuid();

    return p;
}
