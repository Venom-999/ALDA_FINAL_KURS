#ifndef SERVICE_H
#define SERVICE_H

#include <QString>
#include <QUuid>
#include <QStringList>
#include <QJsonObject>
#include <QDateTime>

class Service
{
public:
    Service(const QUuid& id = QUuid::createUuid(),
            const QUuid& providerId = QUuid(),
            const QString& title = "",
            const QString& description = "",
            const QString& category = "",
            double price = 0.0,
            bool active = true);

    Service(const QUuid& id,
            const QUuid& providerId,
            const QString& title,
            const QString& description,
            const QString& category,
            double price,
            bool active,
            double rating);

    // Getters
    QUuid getId() const { return m_id; }
    QUuid getProviderId() const { return m_providerId; }
    QString getTitle() const { return m_title; }
    QString getDescription() const { return m_description; }
    QString getCategory() const { return m_category; }
    double getPrice() const { return m_price; }
    bool isActive() const { return m_active; }
    double getRating() const { return m_rating; }
    QStringList getMedia() const { return m_media; }
    QDateTime getCreatedAt() const { return m_createdAt; }

    // Rating
    void setRating(double rating);

    // Setters
    void setProviderId(const QUuid& providerId);
    void setTitle(const QString& title);
    void setDescription(const QString& description);
    void setCategory(const QString& category);
    void setPrice(double price);
    void setActive(bool active);

    // Media ops
    void addMedia(const QString& path);
    void removeMedia(const QString& path);
    void clearMedia();
    bool hasMedia(const QString& path) const;

    // Info
    QString getInfo() const;
    QString getFullInfo() const;

    // JSON
    QJsonObject toJson() const;
    static Service fromJson(const QJsonObject& json);

private:
    QUuid m_id;
    QUuid m_providerId;

    QString m_title;
    QString m_description;
    QString m_category;

    double m_price = 0.0;
    bool m_active = true;

    double m_rating = 0.0;   // ВАЖНО: поле должно существовать

    QStringList m_media;
    QDateTime m_createdAt;
};

#endif // SERVICE_H
