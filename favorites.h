#ifndef FAVORITES_H
#define FAVORITES_H

#include <QUuid>
#include <QDateTime>
#include <QVector>
#include <QString>
#include <QJsonObject>

class Favorites
{
public:
    Favorites();
    Favorites(const QUuid& favoritesId, const QUuid& userId);

    QUuid favoritesId() const { return m_favoritesId; }
    QUuid userId() const { return m_userId; }
    QDateTime lastUpdated() const { return m_lastUpdated; }

    const QVector<QUuid>& favoriteServiceIds() const { return m_favoriteServiceIds; }
    const QVector<QUuid>& favoriteProviderIds() const { return m_favoriteProviderIds; }
    const QVector<QUuid>& viewedServiceIds() const { return m_viewedServiceIds; }

    void touch(); // lastUpdated = now

    // toggle: если был -> убрать; если не был -> добавить
    bool toggleFavoriteService(const QUuid& serviceId);
    bool toggleFavoriteProvider(const QUuid& providerId);

    // история просмотров: добавить в начало, убрать дубликаты, ограничить размер
    void addViewedService(const QUuid& serviceId, int maxItems = 50);
    void clearViewHistory();

    QString getInfo() const;
    QString getFullInfo() const;

    QJsonObject toJson() const;
    static Favorites fromJson(const QJsonObject& json);

private:
    static int indexOf(const QVector<QUuid>& v, const QUuid& id);

private:
    QUuid m_favoritesId;
    QUuid m_userId;
    QDateTime m_lastUpdated;

    QVector<QUuid> m_favoriteServiceIds;
    QVector<QUuid> m_favoriteProviderIds;
    QVector<QUuid> m_viewedServiceIds;
};

#endif // FAVORITES_H
