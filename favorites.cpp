#include "favorites.h"

#include <QJsonArray>

static QVector<QUuid> uuidsFromJsonArray(const QJsonArray& arr) {
    QVector<QUuid> out;
    out.reserve(arr.size());
    for (const auto& v : arr) {
        const QUuid id(v.toString());
        if (!id.isNull()) out.append(id);
    }
    return out;
}

static QJsonArray uuidsToJsonArray(const QVector<QUuid>& v) {
    QJsonArray arr;
    for (const auto& id : v)
        arr.append(id.toString(QUuid::WithoutBraces));
    return arr;
}

Favorites::Favorites()
    : m_favoritesId(QUuid::createUuid()),
    m_lastUpdated(QDateTime::currentDateTime())
{
}

Favorites::Favorites(const QUuid& favoritesId, const QUuid& userId)
    : m_favoritesId(favoritesId.isNull() ? QUuid::createUuid() : favoritesId),
    m_userId(userId),
    m_lastUpdated(QDateTime::currentDateTime())
{
}

int Favorites::indexOf(const QVector<QUuid>& v, const QUuid& id) {
    for (int i = 0; i < v.size(); ++i)
        if (v[i] == id) return i;
    return -1;
}

void Favorites::touch() {
    m_lastUpdated = QDateTime::currentDateTime();
}

bool Favorites::toggleFavoriteService(const QUuid& serviceId) {
    if (serviceId.isNull()) return false;
    const int idx = indexOf(m_favoriteServiceIds, serviceId);
    if (idx >= 0) {
        m_favoriteServiceIds.removeAt(idx);
        touch();
        return false; // теперь НЕ избранное
    }
    m_favoriteServiceIds.append(serviceId);
    touch();
    return true; // теперь избранное
}

bool Favorites::toggleFavoriteProvider(const QUuid& providerId) {
    if (providerId.isNull()) return false;
    const int idx = indexOf(m_favoriteProviderIds, providerId);
    if (idx >= 0) {
        m_favoriteProviderIds.removeAt(idx);
        touch();
        return false;
    }
    m_favoriteProviderIds.append(providerId);
    touch();
    return true;
}

void Favorites::addViewedService(const QUuid& serviceId, int maxItems) {
    if (serviceId.isNull()) return;
    if (maxItems < 1) maxItems = 1;

    // убрать из текущего места (если уже есть)
    const int idx = indexOf(m_viewedServiceIds, serviceId);
    if (idx >= 0) m_viewedServiceIds.removeAt(idx);

    // добавить в начало
    m_viewedServiceIds.prepend(serviceId);

    // ограничение размера
    while (m_viewedServiceIds.size() > maxItems)
        m_viewedServiceIds.removeLast();

    touch();
}

void Favorites::clearViewHistory() {
    m_viewedServiceIds.clear();
    touch();
}

QString Favorites::getInfo() const {
    return QString("Favorites %1 | services=%2 providers=%3 history=%4")
    .arg(m_favoritesId.toString(QUuid::WithoutBraces).left(8))
        .arg(m_favoriteServiceIds.size())
        .arg(m_favoriteProviderIds.size())
        .arg(m_viewedServiceIds.size());
}

QString Favorites::getFullInfo() const {
    return QString(
               "=== FAVORITES ===\n"
               "favoritesId: %1\n"
               "userId: %2\n"
               "lastUpdated: %3\n"
               "favoriteServices: %4\n"
               "favoriteProviders: %5\n"
               "viewHistory: %6\n"
               )
        .arg(m_favoritesId.toString(QUuid::WithoutBraces))
        .arg(m_userId.toString(QUuid::WithoutBraces))
        .arg(m_lastUpdated.toString(Qt::ISODate))
        .arg(m_favoriteServiceIds.size())
        .arg(m_favoriteProviderIds.size())
        .arg(m_viewedServiceIds.size());
}

QJsonObject Favorites::toJson() const {
    QJsonObject j;
    j["favoritesId"] = m_favoritesId.toString(QUuid::WithoutBraces);
    j["userId"] = m_userId.toString(QUuid::WithoutBraces);
    j["lastUpdated"] = m_lastUpdated.toString(Qt::ISODate);

    j["favoriteServiceIds"] = uuidsToJsonArray(m_favoriteServiceIds);
    j["favoriteProviderIds"] = uuidsToJsonArray(m_favoriteProviderIds);
    j["viewedServiceIds"] = uuidsToJsonArray(m_viewedServiceIds);
    return j;
}

Favorites Favorites::fromJson(const QJsonObject& json) {
    Favorites f(
        QUuid(json.value("favoritesId").toString()),
        QUuid(json.value("userId").toString())
        );

    f.m_lastUpdated = QDateTime::fromString(json.value("lastUpdated").toString(), Qt::ISODate);
    if (!f.m_lastUpdated.isValid())
        f.m_lastUpdated = QDateTime::currentDateTime();

    f.m_favoriteServiceIds = uuidsFromJsonArray(json.value("favoriteServiceIds").toArray());
    f.m_favoriteProviderIds = uuidsFromJsonArray(json.value("favoriteProviderIds").toArray());
    f.m_viewedServiceIds = uuidsFromJsonArray(json.value("viewedServiceIds").toArray());

    if (f.m_favoritesId.isNull()) f.m_favoritesId = QUuid::createUuid();
    return f;
}
