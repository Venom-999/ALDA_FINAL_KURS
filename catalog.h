#ifndef CATALOG_H
#define CATALOG_H

#include <QVector>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QUuid>

#include "service.h" // Service хранится по значению -> нужен полный тип [file:36]

class Catalog
{
public:
    Catalog();

    // Service operations
    void addService(const Service& service);
    bool removeService(const QUuid& serviceId);
    bool updateService(const Service& service); // удобно, чтобы не делать remove+add снаружи

    // Search operations
    QVector<Service> searchByName(const QString& name) const;
    QVector<Service> searchByDescription(const QString& text) const;

    // Filter operations
    QVector<Service> filterByCategory(const QString& category) const;
    QVector<Service> filterByPrice(double minPrice, double maxPrice) const;
    QVector<Service> filterByRating(double minRating) const;
    QVector<Service> getActiveServices() const;

    // Get special lists
    QVector<Service> getPopularServices(int count = 10) const;
    QVector<Service> getNewServices(int count = 10) const;

    QVector<Service> getAllServices() const { return m_services; }
    QStringList getCategories() const { return m_categories; }
    QStringList getSearchHistory() const { return m_searchHistory; }

    // Search history
    void addSearchHistory(const QString& query);

    // Info methods
    QString getInfo() const;
    QString getFullInfo() const;

    // JSON
    QJsonObject toJson() const;
    static Catalog fromJson(const QJsonObject& json);

private:
    int indexOf(const QUuid& id) const;
    void ensureCategory(const QString& category);

private:
    QVector<Service> m_services;
    QStringList m_categories;
    QStringList m_searchHistory;
};

#endif // CATALOG_H
