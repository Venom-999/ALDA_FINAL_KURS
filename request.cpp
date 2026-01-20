#include "request.h"

#include <QtGlobal>
#include <QJsonValue>

Request::Request(const QUuid &id,
                 const QUuid &serviceId,
                 const QUuid &clientId,
                 const QUuid &providerId)
    : m_id(id.isNull() ? QUuid::createUuid() : id),
    m_serviceId(serviceId),
    m_clientId(clientId),
    m_providerId(providerId),
    m_createdAt(QDateTime::currentDateTime())
{
}

void Request::setDescription(const QString &description)
{
    m_description = description.trimmed();
}

void Request::updateStatus(Status newStatus)
{
    if (m_status == newStatus)
        return;

    m_status = newStatus;

    // completedAt фиксируем только при первом переходе в Completed
    if (newStatus == Status::Completed && !m_completedAt.isValid())
        m_completedAt = QDateTime::currentDateTime();
}

void Request::addComment(const QString &comment)
{
    const QString c = comment.trimmed();
    if (c.isEmpty())
        return;

    m_comments.append(c);
}

QString Request::getStatusString() const
{
    switch (m_status) {
    case Status::Pending:    return "Pending";
    case Status::Accepted:   return "Accepted";
    case Status::InProgress: return "InProgress";
    case Status::Completed:  return "Completed";
    case Status::Cancelled:  return "Cancelled";
    }
    return "Pending";
}

Request::Status Request::statusFromString(const QString &s)
{
    const QString x = s.trimmed().toLower();

    if (x == "pending") return Status::Pending;
    if (x == "accepted") return Status::Accepted;
    if (x == "inprogress" || x == "in progress") return Status::InProgress;
    if (x == "completed") return Status::Completed;
    if (x == "cancelled" || x == "canceled") return Status::Cancelled;

    return Status::Pending;
}

Request::Status Request::statusFromInt(int v)
{
    if (v < 0) v = 0;
    if (v > 4) v = 4;
    return static_cast<Status>(v);
}

QString Request::getInfo() const
{
    return QString("Request %1 | %2")
    .arg(m_id.toString(QUuid::WithoutBraces).left(8))
        .arg(getStatusString());
}

QString Request::getFullInfo() const
{
    return QString(
               "=== REQUEST ===\n"
               "id: %1\n"
               "serviceId: %2\n"
               "clientId: %3\n"
               "providerId: %4\n"
               "status: %5\n"
               "description: %6\n"
               "createdAt: %7\n"
               "completedAt: %8\n"
               "comments: %9\n")
        .arg(m_id.toString(QUuid::WithoutBraces))
        .arg(m_serviceId.toString(QUuid::WithoutBraces))
        .arg(m_clientId.toString(QUuid::WithoutBraces))
        .arg(m_providerId.toString(QUuid::WithoutBraces))
        .arg(getStatusString())
        .arg(m_description)
        .arg(m_createdAt.toString(Qt::ISODate))
        .arg(m_completedAt.isValid() ? m_completedAt.toString(Qt::ISODate) : QString())
        .arg(m_comments.size());
}

QJsonObject Request::toJson() const
{
    QJsonObject json;
    json["id"] = m_id.toString(QUuid::WithoutBraces);
    json["serviceId"] = m_serviceId.toString(QUuid::WithoutBraces);
    json["clientId"] = m_clientId.toString(QUuid::WithoutBraces);
    json["providerId"] = m_providerId.toString(QUuid::WithoutBraces);

    json["description"] = m_description;

    // В DataManager статус передаётся как int index (0..4), поэтому храним числом.
    json["status"] = static_cast<int>(m_status);

    json["createdAt"] = m_createdAt.toString(Qt::ISODate);
    json["completedAt"] = m_completedAt.isValid() ? m_completedAt.toString(Qt::ISODate) : QString();

    QJsonArray commentsArray;
    for (const QString &c : m_comments)
        commentsArray.append(c);
    json["comments"] = commentsArray;

    return json;
}

Request Request::fromJson(const QJsonObject &json)
{
    const QUuid id(json.value("id").toString());
    const QUuid serviceId(json.value("serviceId").toString());
    const QUuid clientId(json.value("clientId").toString());
    const QUuid providerId(json.value("providerId").toString());

    Request r(id, serviceId, clientId, providerId);

    r.m_description = json.value("description").toString();

    const QJsonValue st = json.value("status");
    if (st.isDouble())
        r.m_status = statusFromInt(st.toInt());
    else if (st.isString())
        r.m_status = statusFromString(st.toString());
    else
        r.m_status = Status::Pending;

    r.m_createdAt = QDateTime::fromString(json.value("createdAt").toString(), Qt::ISODate);
    if (!r.m_createdAt.isValid())
        r.m_createdAt = QDateTime::currentDateTime();

    r.m_completedAt = QDateTime::fromString(json.value("completedAt").toString(), Qt::ISODate);

    const QJsonArray commentsArray = json.value("comments").toArray();
    for (const QJsonValue &v : commentsArray) {
        const QString c = v.toString().trimmed();
        if (!c.isEmpty())
            r.m_comments.append(c);
    }

    // если пришёл Completed, но completedAt пустой — можно восстановить по createdAt (или "сейчас")
    if (r.m_status == Status::Completed && !r.m_completedAt.isValid())
        r.m_completedAt = r.m_createdAt;

    return r;
}
