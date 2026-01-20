#ifndef PROFILE_H
#define PROFILE_H

#include <QUuid>
#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Profile
{
public:
    Profile(const QUuid& profileId = QUuid::createUuid(),
            const QUuid& ownerUserId = QUuid(),
            const QString& name = "",
            const QString& description = "");

    // Getters
    QUuid profileId() const { return m_profileId; }
    QUuid ownerUserId() const { return m_ownerUserId; }

    QString name() const { return m_name; }
    QString description() const { return m_description; }

    QString avatarPath() const { return m_avatarPath; }

    QString contactEmail() const { return m_contactEmail; }
    QString contactPhone() const { return m_contactPhone; }

    float rating() const { return m_rating; }
    int reviewCount() const { return m_reviewCount; }

    bool isVerified() const { return m_isVerified; }
    QDateTime createdAt() const { return m_createdAt; }

    // Setters
    void setOwnerUserId(const QUuid& id);
    void setName(const QString& name);
    void setDescription(const QString& description);
    void setAvatarPath(const QString& path);
    void setContactEmail(const QString& email);
    void setContactPhone(const QString& phone);
    void setVerified(bool verified);

    // Rating
    void addRating(float r); // 0..5

    // JSON
    QJsonObject toJson() const;
    static Profile fromJson(const QJsonObject& json);

private:
    QUuid m_profileId;
    QUuid m_ownerUserId;

    QString m_name;
    QString m_description;
    QString m_avatarPath;

    QString m_contactEmail;
    QString m_contactPhone;

    float m_rating = 0.0f;
    int m_reviewCount = 0;

    QDateTime m_createdAt;
    bool m_isVerified = false;
};

#endif // PROFILE_H
