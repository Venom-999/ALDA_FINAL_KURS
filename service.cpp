#include "service.h"

#include <QJsonArray>
#include <QtGlobal>

static double clampRating(double r)
{
    if (r < 0.0) return 0.0;
    if (r > 5.0) return 5.0;
    return r;
}

Service::Service(const QUuid& id,
                 const QUuid& providerId,
                 const QString& title,
                 const QString& description,
                 const QString& category,
                 double price,
                 bool active)
    : m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_providerId(providerId)
    , m_title(title)
    , m_description(description)
    , m_category(category)
    , m_price(price < 0.0 ? 0.0 : price)
    , m_active(active)
    , m_rating(0.0)
    , m_createdAt(QDateTime::currentDateTime())
{}

Service::Service(const QUuid& id,
                 const QUuid& providerId,
                 const QString& title,
                 const QString& description,
                 const QString& category,
                 double price,
                 bool active,
                 double rating)
    : m_id(id.isNull() ? QUuid::createUuid() : id)
    , m_providerId(providerId)
    , m_title(title)
    , m_description(description)
    , m_category(category)
    , m_price(price < 0.0 ? 0.0 : price)
    , m_active(active)
    , m_rating(clampRating(rating))
    , m_createdAt(QDateTime::currentDateTime())
{}

// rating
void Service::setRating(double rating)
{
    m_rating = clampRating(rating);
}

// setters
void Service::setProviderId(const QUuid& providerId) { m_providerId = providerId; }
void Service::setTitle(const QString& title) { m_title = title; }
void Service::setDescription(const QString& description) { m_description = description; }
void Service::setCategory(const QString& category) { m_category = category; }

void Service::setPrice(double price)
{
    m_price = (price < 0.0) ? 0.0 : price;
}

void Service::setActive(bool active) { m_active = active; }

// media
void Service::addMedia(const QString& path)
{
    const QString p = path.trimmed();
    if (p.isEmpty()) return;
    if (!m_media.contains(p)) m_media.append(p);
}

void Service::removeMedia(const QString& path)
{
    m_media.removeAll(path.trimmed());
}

void Service::clearMedia()
{
    m_media.clear();
}

bool Service::hasMedia(const QString& path) const
{
    return m_media.contains(path);
}

// info
QString Service::getInfo() const
{
    return QString("%1 | %2 ₽ | %3★")
        .arg(m_title.isEmpty() ? "Untitled" : m_title)
        .arg(QString::number(m_price, 'f', 2))
        .arg(QString::number(m_rating, 'f', 1));
}

QString Service::getFullInfo() const
{
    return QString(
               "Service:\n"
               "  id: %1\n"
               "  providerId: %2\n"
               "  title: %3\n"
               "  category: %4\n"
               "  price: %5\n"
               "  active: %6\n"
               "  rating: %7\n"
               "  media: %8\n"
               "  createdAt: %9\n"
               "  description: %10\n")
        .arg(m_id.toString(QUuid::WithoutBraces))
        .arg(m_providerId.toString(QUuid::WithoutBraces))
        .arg(m_title)
        .arg(m_category)
        .arg(QString::number(m_price, 'f', 2))
        .arg(m_active ? "true" : "false")
        .arg(QString::number(m_rating, 'f', 2))
        .arg(m_media.join(", "))
        .arg(m_createdAt.toString(Qt::ISODate))
        .arg(m_description);
}

// json
QJsonObject Service::toJson() const
{
    QJsonObject json;
    json["id"] = m_id.toString(QUuid::WithoutBraces);
    json["providerId"] = m_providerId.toString(QUuid::WithoutBraces);
    json["title"] = m_title;
    json["description"] = m_description;
    json["category"] = m_category;
    json["price"] = m_price;
    json["active"] = m_active;
    json["rating"] = m_rating;
    json["createdAt"] = m_createdAt.toString(Qt::ISODate);

    QJsonArray mediaArr;
    for (const auto& p : m_media) mediaArr.append(p);
    json["media"] = mediaArr;

    return json;
}

Service Service::fromJson(const QJsonObject& json)
{
    Service s(
        QUuid(json.value("id").toString()),
        QUuid(json.value("providerId").toString()),
        json.value("title").toString(),
        json.value("description").toString(),
        json.value("category").toString(),
        json.value("price").toDouble(0.0),
        json.value("active").toBool(true),
        json.value("rating").toDouble(0.0)
        );

    s.m_createdAt = QDateTime::fromString(json.value("createdAt").toString(), Qt::ISODate);

    const QJsonArray mediaArr = json.value("media").toArray();
    for (const auto& v : mediaArr) s.m_media.append(v.toString());

    return s;
}
