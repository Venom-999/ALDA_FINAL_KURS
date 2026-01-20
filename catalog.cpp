#include "catalog.h"

#include <QJsonArray>
#include <QDateTime>

Catalog::Catalog()
{
    m_categories << "Бытовые услуги" << "Дизайн" << "Ремонт"
                 << "Обучение" << "Консультирование" << "Программирование";
}

int Catalog::indexOf(const QUuid& id) const
{
    if (id.isNull()) return -1;
    for (int i = 0; i < m_services.size(); ++i)
        if (m_services[i].getId() == id) return i;
    return -1;
}

void Catalog::ensureCategory(const QString& category)
{
    const QString c = category.trimmed();
    if (c.isEmpty()) return;
    if (!m_categories.contains(c)) m_categories.append(c);
}

void Catalog::addService(const Service& service)
{
    // если пришёл сервис с уже существующим id — заменяем
    const int idx = indexOf(service.getId());
    if (idx >= 0) m_services[idx] = service;
    else m_services.append(service);

    ensureCategory(service.getCategory());
}

bool Catalog::removeService(const QUuid& serviceId)
{
    const int idx = indexOf(serviceId);
    if (idx < 0) return false;
    m_services.removeAt(idx);
    return true;
}

bool Catalog::updateService(const Service& service)
{
    const int idx = indexOf(service.getId());
    if (idx < 0) return false;
    m_services[idx] = service;
    ensureCategory(service.getCategory());
    return true;
}

QVector<Service> Catalog::searchByName(const QString& name) const
{
    QVector<Service> results;
    const QString q = name.trimmed();
    if (q.isEmpty()) return results;

    for (const auto& s : m_services)
        if (s.getTitle().contains(q, Qt::CaseInsensitive))
            results.append(s);

    return results;
}

QVector<Service> Catalog::searchByDescription(const QString& text) const
{
    QVector<Service> results;
    const QString q = text.trimmed();
    if (q.isEmpty()) return results;

    for (const auto& s : m_services)
        if (s.getDescription().contains(q, Qt::CaseInsensitive))
            results.append(s);

    return results;
}

QVector<Service> Catalog::filterByCategory(const QString& category) const
{
    QVector<Service> results;
    const QString c = category.trimmed();
    if (c.isEmpty()) return results;

    for (const auto& s : m_services)
        if (s.getCategory() == c)
            results.append(s);

    return results;
}

QVector<Service> Catalog::filterByPrice(double minPrice, double maxPrice) const
{
    QVector<Service> results;
    if (minPrice > maxPrice) return results;

    for (const auto& s : m_services) {
        const double p = s.getPrice();
        if (p >= minPrice && p <= maxPrice)
            results.append(s);
    }
    return results;
}

QVector<Service> Catalog::filterByRating(double minRating) const
{
    QVector<Service> results;
    for (const auto& s : m_services)
        if (s.getRating() >= minRating)
            results.append(s);
    return results;
}

QVector<Service> Catalog::getActiveServices() const
{
    QVector<Service> results;
    for (const auto& s : m_services)
        if (s.isActive())
            results.append(s);
    return results;
}

// пузырёк по рейтингу (без lambda, как у тебя) [file:55]
QVector<Service> Catalog::getPopularServices(int count) const
{
    QVector<Service> sorted = m_services;

    for (int i = 0; i < sorted.size() - 1; ++i) {
        for (int j = i + 1; j < sorted.size(); ++j) {
            if (sorted[i].getRating() < sorted[j].getRating()) {
                Service tmp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = tmp;
            }
        }
    }

    if (count < 0) count = 0;
    if (sorted.size() > count) sorted = sorted.mid(0, count);
    return sorted;
}

// пузырёк по дате (без lambda, как у тебя) [file:55]
QVector<Service> Catalog::getNewServices(int count) const
{
    QVector<Service> sorted = m_services;

    for (int i = 0; i < sorted.size() - 1; ++i) {
        for (int j = i + 1; j < sorted.size(); ++j) {
            if (sorted[i].getCreatedAt() < sorted[j].getCreatedAt()) {
                Service tmp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = tmp;
            }
        }
    }

    if (count < 0) count = 0;
    if (sorted.size() > count) sorted = sorted.mid(0, count);
    return sorted;
}

void Catalog::addSearchHistory(const QString& query)
{
    const QString q = query.trimmed();
    if (q.isEmpty()) return;

    m_searchHistory.removeAll(q);
    m_searchHistory.prepend(q);
    if (m_searchHistory.size() > 50) m_searchHistory.removeLast();
}

QString Catalog::getInfo() const
{
    return QString("Каталог: %1 услуг в %2 категориях")
        .arg(m_services.size())
        .arg(m_categories.size());
}

QString Catalog::getFullInfo() const
{
    return QString("=== КАТАЛОГ ===\n"
                   "Услуг: %1\n"
                   "Категорий: %2\n"
                   "История поиска: %3")
        .arg(m_services.size())
        .arg(m_categories.size())
        .arg(m_searchHistory.size());
}

QJsonObject Catalog::toJson() const
{
    QJsonObject json;

    QJsonArray servicesArray;
    for (const auto& s : m_services)
        servicesArray.append(s.toJson()); // Service::toJson [file:37]
    json["services"] = servicesArray;

    QJsonArray categoriesArray;
    for (const auto& c : m_categories)
        categoriesArray.append(c);
    json["categories"] = categoriesArray;

    QJsonArray historyArray;
    for (const auto& h : m_searchHistory)
        historyArray.append(h);
    json["searchHistory"] = historyArray;

    return json;
}

Catalog Catalog::fromJson(const QJsonObject& json)
{
    Catalog catalog;

    catalog.m_categories.clear();
    const QJsonArray categoriesArray = json.value("categories").toArray();
    for (int i = 0; i < categoriesArray.size(); ++i)
        catalog.m_categories.append(categoriesArray[i].toString());

    catalog.m_searchHistory.clear();
    const QJsonArray historyArray = json.value("searchHistory").toArray();
    for (int i = 0; i < historyArray.size(); ++i)
        catalog.m_searchHistory.append(historyArray[i].toString());

    catalog.m_services.clear();
    const QJsonArray servicesArray = json.value("services").toArray();
    for (int i = 0; i < servicesArray.size(); ++i)
        catalog.m_services.append(Service::fromJson(servicesArray[i].toObject())); // [file:37]

    return catalog;
}
