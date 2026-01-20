#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <QUuid>
#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Subscription
{
public:
    Subscription();
    Subscription(const QUuid& subscriptionId, const QUuid& userId);

    // getters
    QUuid subscriptionId() const { return m_subscriptionId; }
    QUuid userId() const { return m_userId; }
    QString planType() const { return m_planType; }
    double price() const { return m_price; }
    QDateTime startDate() const { return m_startDate; }
    QDateTime endDate() const { return m_endDate; }
    bool active() const { return m_active; }

    // setters (валидируем аккуратно)
    void setSubscriptionId(const QUuid& id);
    void setUserId(const QUuid& id);

    void setPlanType(const QString& planType);
    void setPrice(double price);

    // ожидаем ISO: "2026-01-20T12:00:00" или "2026-01-20"
    void setStartDate(const QDateTime& dt);
    void setEndDate(const QDateTime& dt);

    void setActive(bool on);

    // бизнес-методы
    bool isExpired(const QDateTime& now = QDateTime::currentDateTime()) const;
    bool isValid() const; // минимальная проверка целостности

    void cancel(); // active=false, endDate=now если endDate пустая/в будущем

    QString getInfo() const;
    QString getFullInfo() const;

    QJsonObject toJson() const;
    static Subscription fromJson(const QJsonObject& json);

private:
    QUuid m_subscriptionId;
    QUuid m_userId;
    QString m_planType;
    double m_price = 0.0;
    QDateTime m_startDate;
    QDateTime m_endDate;
    bool m_active = false;
};

#endif // SUBSCRIPTION_H
