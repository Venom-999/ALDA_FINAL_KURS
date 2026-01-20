#include "subscription.h"

#include <QtGlobal>
#include <QJsonValue>
#include <QDebug>

static QDateTime safeIsoDt(const QString& s) {
    auto dt = QDateTime::fromString(s.trimmed(), Qt::ISODate);
    if (!dt.isValid()) {
        // допускаем формат "YYYY-MM-DD"
        dt = QDateTime::fromString(s.trimmed() + "T00:00:00", Qt::ISODate);
    }
    return dt;
}

Subscription::Subscription()
    : m_subscriptionId(QUuid::createUuid())
{
}

Subscription::Subscription(const QUuid& subscriptionId, const QUuid& userId)
    : m_subscriptionId(subscriptionId.isNull() ? QUuid::createUuid() : subscriptionId),
    m_userId(userId)
{
}

void Subscription::setSubscriptionId(const QUuid& id) {
    m_subscriptionId = id.isNull() ? QUuid::createUuid() : id;
}

void Subscription::setUserId(const QUuid& id) {
    m_userId = id;
}

void Subscription::setPlanType(const QString& planType) {
    m_planType = planType.trimmed();
}

void Subscription::setPrice(double price) {
    if (!qIsFinite(price) || price < 0) price = 0;
    m_price = price;
}

void Subscription::setStartDate(const QDateTime& dt) {
    m_startDate = dt;
}

void Subscription::setEndDate(const QDateTime& dt) {
    m_endDate = dt;
}

void Subscription::setActive(bool on) {
    m_active = on;
}

bool Subscription::isExpired(const QDateTime& now) const {
    if (!m_endDate.isValid()) return false;
    return now >= m_endDate;
}

bool Subscription::isValid() const {
    if (m_subscriptionId.isNull()) return false;
    if (m_userId.isNull()) return false;
    if (m_planType.trimmed().isEmpty()) return false;
    if (m_price < 0) return false;
    if (m_startDate.isValid() && m_endDate.isValid() && m_endDate < m_startDate) return false;
    return true;
}

void Subscription::cancel() {
    m_active = false;
    const auto now = QDateTime::currentDateTime();
    // если endDate не задана или в будущем — делаем "закрытие" сейчас
    if (!m_endDate.isValid() || m_endDate > now) m_endDate = now;
}

QString Subscription::getInfo() const {
    return QString("Subscription %1 | %2 | %3")
    .arg(m_subscriptionId.toString(QUuid::WithoutBraces).left(8))
        .arg(m_planType.isEmpty() ? "NoPlan" : m_planType)
        .arg(m_active ? "active" : "inactive");
}

QString Subscription::getFullInfo() const {
    return QString(
               "=== SUBSCRIPTION ===\n"
               "subscriptionId: %1\n"
               "userId: %2\n"
               "planType: %3\n"
               "price: %4\n"
               "startDate: %5\n"
               "endDate: %6\n"
               "active: %7\n"
               )
        .arg(m_subscriptionId.toString(QUuid::WithoutBraces))
        .arg(m_userId.toString(QUuid::WithoutBraces))
        .arg(m_planType)
        .arg(QString::number(m_price, 'f', 2))
        .arg(m_startDate.isValid() ? m_startDate.toString(Qt::ISODate) : QString())
        .arg(m_endDate.isValid() ? m_endDate.toString(Qt::ISODate) : QString())
        .arg(m_active ? "true" : "false");
}

QJsonObject Subscription::toJson() const {
    QJsonObject j;
    j["subscriptionId"] = m_subscriptionId.toString(QUuid::WithoutBraces);
    j["userId"] = m_userId.toString(QUuid::WithoutBraces);
    j["planType"] = m_planType;
    j["price"] = m_price;
    j["startDate"] = m_startDate.isValid() ? m_startDate.toString(Qt::ISODate) : QString();
    j["endDate"] = m_endDate.isValid() ? m_endDate.toString(Qt::ISODate) : QString();
    j["active"] = m_active;
    return j;
}

Subscription Subscription::fromJson(const QJsonObject& json) {
    Subscription s(
        QUuid(json.value("subscriptionId").toString()),
        QUuid(json.value("userId").toString())
        );

    s.m_planType = json.value("planType").toString();
    s.m_price = json.value("price").toDouble(0.0);

    s.m_startDate = safeIsoDt(json.value("startDate").toString());
    s.m_endDate   = safeIsoDt(json.value("endDate").toString());

    const auto act = json.value("active");
    s.m_active = act.isBool() ? act.toBool() : (act.toString().trimmed().toLower() == "true");

    if (s.m_subscriptionId.isNull()) s.m_subscriptionId = QUuid::createUuid();
    return s;
}
