#include "datamanager.h"

#include <QCryptographicHash>
#include <QRandomGenerator>

#include <QFile>
#include <QDir>
#include <QStandardPaths>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// ---------------- paths ----------------
static QString appDataDir()
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir;
}

static QString servicesFilePath()       { return appDataDir() + "/services.json"; }
static QString requestsFilePath()       { return appDataDir() + "/requests.json"; }
static QString reviewsFilePath()        { return appDataDir() + "/reviews.json"; }
static QString subscriptionsFilePath()  { return appDataDir() + "/subscriptions.json"; }
static QString favoritesFilePath()      { return appDataDir() + "/favorites.json"; }

// ---------------- local helpers ----------------
static int findProfileByOwner(const QVector<Profile>& profiles, const QUuid& ownerId)
{
    for (int i = 0; i < profiles.size(); ++i)
        if (profiles[i].ownerUserId() == ownerId)
            return i;
    return -1;
}

static int indexOfSubscriptionByUser(const QVector<Subscription>& v, const QUuid& uid)
{
    for (int i = 0; i < v.size(); ++i)
        if (v[i].userId() == uid)
            return i;
    return -1;
}

static int indexOfFavoritesByUser(const QVector<Favorites>& v, const QUuid& uid)
{
    for (int i = 0; i < v.size(); ++i)
        if (v[i].userId() == uid)
            return i;
    return -1;
}

static Favorites& ensureFavoritesForUser(QVector<Favorites>& v, const QUuid& uid)
{
    int idx = indexOfFavoritesByUser(v, uid);
    if (idx < 0) {
        v.append(Favorites(QUuid::createUuid(), uid));
        idx = v.size() - 1;
    }
    return v[idx];
}

static QDateTime parseIsoMaybeDateOnly(const QString& s)
{
    const QString t = s.trimmed();
    if (t.isEmpty()) return QDateTime();

    QDateTime dt = QDateTime::fromString(t, Qt::ISODate);
    if (dt.isValid()) return dt;

    // allow YYYY-MM-DD
    dt = QDateTime::fromString(t + "T00:00:00", Qt::ISODate);
    return dt;
}

// ---------------- DataManager ----------------
DataManager::DataManager(QObject* parent)
    : QObject(parent)
{
    loadServices();
    loadRequests();
    loadReviews();
    loadSubscriptions();
    loadFavorites();
}

DataManager& DataManager::instance()
{
    static DataManager s;
    return s;
}

QString DataManager::currentUserRole() const
{
    switch (m_currentUser.roleIndex) {
    case 0: return "Client";
    case 1: return "Provider";
    case 2: return "Admin";
    default: return "Client";
    }
}

QString DataManager::hashPasswordHex(const QString& pass)
{
    const QByteArray h = QCryptographicHash::hash(pass.toUtf8(), QCryptographicHash::Sha256);
    return QString::fromLatin1(h.toHex());
}

QString DataManager::genCode6()
{
    const int v = int(QRandomGenerator::global()->bounded(0, 1000000));
    return QString("%1").arg(v, 6, 10, QLatin1Char('0'));
}

int DataManager::findUserByEmailOrPhone(const QString& s) const
{
    const QString key = s.trimmed();
    if (key.isEmpty()) return -1;

    for (int i = 0; i < m_users.size(); ++i) {
        if (m_users[i].email.compare(key, Qt::CaseInsensitive) == 0) return i;
        if (!m_users[i].phone.isEmpty() && m_users[i].phone == key) return i;
    }
    return -1;
}

// ---------------- Auth/User ----------------
bool DataManager::registerUser(const QString& email,
                               const QString& phone,
                               int roleIndex,
                               const QString& password)
{
    const QString em = email.trimmed();
    const QString ph = phone.trimmed();
    if (em.isEmpty() || password.size() < 6) return false;

    for (const auto& u : m_users) {
        if (u.email.compare(em, Qt::CaseInsensitive) == 0) return false;
        if (!ph.isEmpty() && !u.phone.isEmpty() && u.phone == ph) return false;
    }

    UserRec u;
    u.id = QUuid::createUuid();
    u.email = em;
    u.phone = ph;
    u.roleIndex = qBound(0, roleIndex, 2);
    u.passHashHex = hashPasswordHex(password);
    u.createdAt = QDateTime::currentDateTime();
    u.verified = false;
    u.verificationCode.clear();

    m_users.append(u);
    m_currentUser = u;
    m_loggedIn = true;

    emit currentUserChanged();
    emit loggedInChanged();
    return true;
}

bool DataManager::login(const QString& emailOrPhone, const QString& password)
{
    const int idx = findUserByEmailOrPhone(emailOrPhone);
    if (idx < 0) return false;
    if (m_users[idx].passHashHex != hashPasswordHex(password)) return false;

    m_currentUser = m_users[idx];
    m_loggedIn = true;

    emit currentUserChanged();
    emit loggedInChanged();
    return true;
}

void DataManager::logout()
{
    if (!m_loggedIn) return;
    m_loggedIn = false;
    m_currentUser = UserRec();

    emit currentUserChanged();
    emit loggedInChanged();
}

QString DataManager::issueVerificationCode()
{
    if (!m_loggedIn) return QString();

    const QString code = genCode6();
    for (auto& u : m_users) {
        if (u.id == m_currentUser.id) {
            u.verificationCode = code;
            m_currentUser.verificationCode = code;
            break;
        }
    }
    return code;
}

bool DataManager::verifyAccount(const QString& code)
{
    if (!m_loggedIn) return false;

    const QString c = code.trimmed();
    for (auto& u : m_users) {
        if (u.id != m_currentUser.id) continue;

        if (!u.verificationCode.isEmpty() && u.verificationCode == c) {
            u.verified = true;
            u.verificationCode.clear();
            m_currentUser = u;
            emit currentUserChanged();
            return true;
        }
        return false;
    }
    return false;
}

bool DataManager::changePassword(const QString& oldPass, const QString& newPass)
{
    if (!m_loggedIn) return false;
    if (newPass.size() < 6) return false;

    const QString oldH = hashPasswordHex(oldPass);
    const QString newH = hashPasswordHex(newPass);

    for (auto& u : m_users) {
        if (u.id != m_currentUser.id) continue;
        if (u.passHashHex != oldH) return false;

        u.passHashHex = newH;
        m_currentUser = u;
        emit currentUserChanged();
        return true;
    }
    return false;
}

// ---------------- Profile ----------------
QVariantMap DataManager::getMyProfile() const
{
    QVariantMap out;
    if (!m_loggedIn) return out;

    const QUuid ownerId = m_currentUser.id;
    const int idx = findProfileByOwner(m_profiles, ownerId);

    Profile p;
    if (idx >= 0) p = m_profiles[idx];
    else p = Profile(QUuid::createUuid(), ownerId, "", "");

    out["profileId"] = p.profileId().toString(QUuid::WithoutBraces);
    out["ownerUserId"] = p.ownerUserId().toString(QUuid::WithoutBraces);
    out["name"] = p.name();
    out["description"] = p.description();
    out["avatarPath"] = p.avatarPath();
    out["contactEmail"] = p.contactEmail();
    out["contactPhone"] = p.contactPhone();
    out["verified"] = p.isVerified();
    return out;
}

bool DataManager::saveMyProfile(const QString& name,
                                const QString& description,
                                const QString& avatarPath,
                                const QString& contactEmail,
                                const QString& contactPhone)
{
    if (!m_loggedIn) return false;

    const QUuid ownerId = m_currentUser.id;
    int idx = findProfileByOwner(m_profiles, ownerId);
    if (idx < 0) {
        m_profiles.append(Profile(QUuid::createUuid(), ownerId, "", ""));
        idx = m_profiles.size() - 1;
    }

    Profile p = m_profiles[idx];
    p.setOwnerUserId(ownerId);
    p.setName(name);
    p.setDescription(description);
    p.setAvatarPath(avatarPath);
    p.setContactEmail(contactEmail);
    p.setContactPhone(contactPhone);
    p.setVerified(m_currentUser.verified);

    m_profiles[idx] = p;
    return true;
}

// ---------------- Services storage ----------------
void DataManager::loadServices()
{
    QFile f(servicesFilePath());
    if (!f.open(QIODevice::ReadOnly)) return;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) return;

    m_catalog = Catalog::fromJson(doc.object());
}

void DataManager::saveServices() const
{
    QFile f(servicesFilePath());
    if (!f.open(QIODevice::WriteOnly)) return;

    const QJsonDocument doc(m_catalog.toJson());
    f.write(doc.toJson(QJsonDocument::Indented));
}

QVariantList DataManager::servicesToVariantList(const QVector<Service>& v)
{
    QVariantList out;
    out.reserve(v.size());
    for (const auto& s : v)
        out.append(s.toJson().toVariantMap());
    return out;
}

QVariantList DataManager::getAllServices() const
{
    return servicesToVariantList(m_catalog.getAllServices());
}

bool DataManager::addService(const QVariantMap& serviceMap)
{
    const QJsonObject obj = QJsonObject::fromVariantMap(serviceMap);
    const Service s = Service::fromJson(obj);

    m_catalog.addService(s);
    saveServices();
    emit servicesChanged();
    return true;
}

bool DataManager::updateService(const QVariantMap& serviceMap)
{
    const QJsonObject obj = QJsonObject::fromVariantMap(serviceMap);
    const Service s = Service::fromJson(obj);

    if (!m_catalog.updateService(s)) return false;

    saveServices();
    emit servicesChanged();
    return true;
}

bool DataManager::deleteService(const QString& serviceId)
{
    const QUuid id(serviceId.trimmed());
    if (id.isNull()) return false;

    if (!m_catalog.removeService(id)) return false;

    saveServices();
    emit servicesChanged();
    return true;
}

// ---------------- Catalog wrappers ----------------
QVariantList DataManager::catalogGetActiveServices() const
{
    return servicesToVariantList(m_catalog.getActiveServices());
}

QVariantList DataManager::catalogSearchByName(const QString& name)
{
    m_catalog.addSearchHistory(name);
    return servicesToVariantList(m_catalog.searchByName(name));
}

QVariantList DataManager::catalogSearchByDescription(const QString& text)
{
    m_catalog.addSearchHistory(text);
    return servicesToVariantList(m_catalog.searchByDescription(text));
}

QVariantList DataManager::catalogFilterByCategory(const QString& category) const
{
    return servicesToVariantList(m_catalog.filterByCategory(category));
}

QVariantList DataManager::catalogFilterByPrice(double minPrice, double maxPrice) const
{
    return servicesToVariantList(m_catalog.filterByPrice(minPrice, maxPrice));
}

QVariantList DataManager::catalogFilterByRating(double minRating) const
{
    return servicesToVariantList(m_catalog.filterByRating(minRating));
}

QVariantList DataManager::catalogGetPopularServices(int count) const
{
    return servicesToVariantList(m_catalog.getPopularServices(count));
}

QVariantList DataManager::catalogGetNewServices(int count) const
{
    return servicesToVariantList(m_catalog.getNewServices(count));
}

QStringList DataManager::catalogGetCategories() const
{
    return m_catalog.getCategories();
}

QStringList DataManager::catalogGetSearchHistory() const
{
    return m_catalog.getSearchHistory();
}

QString DataManager::catalogGetInfo() const
{
    return m_catalog.getInfo();
}

QString DataManager::catalogGetFullInfo() const
{
    return m_catalog.getFullInfo();
}

// ---------------- Requests storage ----------------
void DataManager::loadRequests()
{
    QFile f(requestsFilePath());
    if (!f.open(QIODevice::ReadOnly)) return;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return;

    m_requests.clear();
    const QJsonArray arr = doc.array();
    m_requests.reserve(arr.size());

    for (const auto& v : arr) {
        if (!v.isObject()) continue;
        m_requests.append(Request::fromJson(v.toObject()));
    }
}

void DataManager::saveRequests() const
{
    QJsonArray arr;
    for (const auto& r : m_requests)
        arr.append(r.toJson());

    QFile f(requestsFilePath());
    if (!f.open(QIODevice::WriteOnly)) return;

    const QJsonDocument doc(arr);
    f.write(doc.toJson(QJsonDocument::Indented));
}

QVariantList DataManager::requestsToVariantList(const QVector<Request>& v)
{
    QVariantList out;
    out.reserve(v.size());
    for (const auto& r : v)
        out.append(r.toJson().toVariantMap());
    return out;
}

int DataManager::indexOfRequest(const QUuid& id) const
{
    for (int i = 0; i < m_requests.size(); ++i)
        if (m_requests[i].getId() == id) return i;
    return -1;
}

QVariantList DataManager::getAllRequests() const
{
    return requestsToVariantList(m_requests);
}

QString DataManager::createRequest(const QString& serviceId,
                                   const QString& providerId,
                                   const QString& description)
{
    if (!m_loggedIn) return QString();

    QUuid sid(serviceId.trimmed());
    QUuid pid(providerId.trimmed());

    // robust: если пришло не UUID — сгенерим, чтобы не падало
    if (sid.isNull()) sid = QUuid::createUuid();
    if (pid.isNull()) pid = QUuid::createUuid();

    Request r(QUuid::createUuid(), sid, m_currentUser.id, pid);
    r.setDescription(description);

    m_requests.append(r);
    saveRequests();
    emit requestsChanged();

    return r.getId().toString(QUuid::WithoutBraces);
}

bool DataManager::deleteRequest(const QString& requestId)
{
    const QUuid rid(requestId.trimmed());
    if (rid.isNull()) return false;

    const int idx = indexOfRequest(rid);
    if (idx < 0) return false;

    m_requests.removeAt(idx);
    saveRequests();
    emit requestsChanged();
    return true;
}

bool DataManager::updateRequestStatus(const QString& requestId, int statusIndex)
{
    if (statusIndex < 0) statusIndex = 0;
    if (statusIndex > 4) statusIndex = 4;

    const QString ridStr = requestId.trimmed();
    if (ridStr.isEmpty()) return false;

    QUuid rid(ridStr);
    int idx = -1;

    if (!rid.isNull()) {
        idx = indexOfRequest(rid);
    } else {
        // fallback: если в QML прилетела строка без UUID-формата
        for (int i = 0; i < m_requests.size(); ++i) {
            if (m_requests[i].getId().toString(QUuid::WithoutBraces) == ridStr) {
                idx = i;
                break;
            }
        }
    }

    if (idx < 0) return false;

    m_requests[idx].setStatusFromInt(statusIndex);
    saveRequests();
    emit requestsChanged();
    return true;
}

bool DataManager::updateRequestDescription(const QString& requestId, const QString& description)
{
    const QUuid rid(requestId.trimmed());
    if (rid.isNull()) return false;

    const int idx = indexOfRequest(rid);
    if (idx < 0) return false;

    m_requests[idx].setDescription(description);
    saveRequests();
    emit requestsChanged();
    return true;
}

bool DataManager::addRequestComment(const QString& requestId, const QString& comment)
{
    const QUuid rid(requestId.trimmed());
    if (rid.isNull()) return false;

    const QString c = comment.trimmed();
    if (c.isEmpty()) return false;

    const int idx = indexOfRequest(rid);
    if (idx < 0) return false;

    m_requests[idx].addComment(c);
    saveRequests();
    emit requestsChanged();
    return true;
}

// ---------------- Reviews storage ----------------
void DataManager::loadReviews()
{
    QFile f(reviewsFilePath());
    if (!f.open(QIODevice::ReadOnly)) return;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return;

    m_reviews.clear();
    const QJsonArray arr = doc.array();
    m_reviews.reserve(arr.size());

    for (const auto& v : arr) {
        if (!v.isObject()) continue;
        m_reviews.append(Review::fromJson(v.toObject()));
    }
}

void DataManager::saveReviews() const
{
    QJsonArray arr;
    for (const auto& r : m_reviews)
        arr.append(r.toJson());

    QFile f(reviewsFilePath());
    if (!f.open(QIODevice::WriteOnly)) return;

    const QJsonDocument doc(arr);
    f.write(doc.toJson(QJsonDocument::Indented));
}

QVariantList DataManager::getReviewsForService(const QString& serviceId) const
{
    QVariantList out;

    const QUuid sid(serviceId.trimmed());
    if (sid.isNull()) return out;

    for (const auto& r : m_reviews) {
        if (r.getServiceId() != sid) continue;
        out.append(r.toJson().toVariantMap());
    }
    return out;
}

bool DataManager::addReview(const QString& serviceId, int rating, const QString& comment)
{
    if (!m_loggedIn) return false;

    const QUuid sid(serviceId.trimmed());
    if (sid.isNull()) return false;

    const QString c = comment.trimmed();
    if (c.isEmpty()) return false;

    int r = rating;
    if (r < 1) r = 1;
    if (r > 5) r = 5;

    m_reviews.append(Review(QUuid::createUuid(), m_currentUser.id, sid, double(r), c));
    saveReviews();
    emit reviewsChanged();
    return true;
}

// ---------------- Subscription storage ----------------
void DataManager::loadSubscriptions()
{
    QFile f(subscriptionsFilePath());
    if (!f.open(QIODevice::ReadOnly)) return;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return;

    m_subscriptions.clear();
    const QJsonArray arr = doc.array();
    m_subscriptions.reserve(arr.size());

    for (const auto& v : arr) {
        if (!v.isObject()) continue;
        m_subscriptions.append(Subscription::fromJson(v.toObject()));
    }
}

void DataManager::saveSubscriptions() const
{
    QJsonArray arr;
    for (const auto& s : m_subscriptions)
        arr.append(s.toJson());

    QFile f(subscriptionsFilePath());
    if (!f.open(QIODevice::WriteOnly)) return;

    const QJsonDocument doc(arr);
    f.write(doc.toJson(QJsonDocument::Indented));
}

// ---------------- Favorites storage ----------------
void DataManager::loadFavorites()
{
    QFile f(favoritesFilePath());
    if (!f.open(QIODevice::ReadOnly)) return;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return;

    m_favorites.clear();
    const QJsonArray arr = doc.array();
    m_favorites.reserve(arr.size());

    for (const auto& v : arr) {
        if (!v.isObject()) continue;
        m_favorites.append(Favorites::fromJson(v.toObject()));
    }
}

void DataManager::saveFavorites() const
{
    QJsonArray arr;
    for (const auto& fav : m_favorites)
        arr.append(fav.toJson());

    QFile f(favoritesFilePath());
    if (!f.open(QIODevice::WriteOnly)) return;

    const QJsonDocument doc(arr);
    f.write(doc.toJson(QJsonDocument::Indented));
}

// ---------------- Subscription API ----------------
QVariantMap DataManager::getMySubscription() const
{
    QVariantMap out;
    if (!m_loggedIn) return out;

    const int idx = indexOfSubscriptionByUser(m_subscriptions, m_currentUser.id);
    if (idx < 0) return out;

    const Subscription& s = m_subscriptions[idx];
    out["subscriptionId"] = s.subscriptionId().toString(QUuid::WithoutBraces);
    out["userId"] = s.userId().toString(QUuid::WithoutBraces);
    out["planType"] = s.planType();
    out["price"] = s.price();
    out["startDate"] = s.startDate().isValid() ? s.startDate().toString(Qt::ISODate) : "";
    out["endDate"] = s.endDate().isValid() ? s.endDate().toString(Qt::ISODate) : "";
    out["active"] = s.active();
    out["info"] = s.getInfo();
    out["fullInfo"] = s.getFullInfo();
    return out;
}

bool DataManager::saveMySubscription(const QString& planType,
                                     double price,
                                     const QString& startIso,
                                     const QString& endIso,
                                     bool active)
{
    if (!m_loggedIn) return false;

    const QDateTime dtStart = parseIsoMaybeDateOnly(startIso);
    const QDateTime dtEnd = parseIsoMaybeDateOnly(endIso);

    int idx = indexOfSubscriptionByUser(m_subscriptions, m_currentUser.id);

    if (idx < 0) {
        Subscription s(QUuid::createUuid(), m_currentUser.id);
        s.setPlanType(planType);
        s.setPrice(price);
        s.setStartDate(dtStart);
        s.setEndDate(dtEnd);
        s.setActive(active);

        if (!s.isValid()) return false;
        m_subscriptions.append(s);
    } else {
        Subscription& s = m_subscriptions[idx];
        s.setPlanType(planType);
        s.setPrice(price);
        s.setStartDate(dtStart);
        s.setEndDate(dtEnd);
        s.setActive(active);

        if (!s.isValid()) return false;
    }

    saveSubscriptions();
    emit subscriptionsChanged();
    return true;
}

bool DataManager::cancelMySubscription()
{
    if (!m_loggedIn) return false;

    const int idx = indexOfSubscriptionByUser(m_subscriptions, m_currentUser.id);
    if (idx < 0) return false;

    m_subscriptions[idx].cancel();

    saveSubscriptions();
    emit subscriptionsChanged();
    return true;
}

// ---------------- Favorites API ----------------
QVariantMap DataManager::getMyFavorites() const
{
    QVariantMap out;
    if (!m_loggedIn) return out;

    const int idx = indexOfFavoritesByUser(m_favorites, m_currentUser.id);
    if (idx < 0) return out;

    const Favorites& f = m_favorites[idx];

    out["favoritesId"] = f.favoritesId().toString(QUuid::WithoutBraces);
    out["userId"] = f.userId().toString(QUuid::WithoutBraces);
    out["lastUpdated"] = f.lastUpdated().toString(Qt::ISODate);

    QVariantList favServices;
    for (const auto& id : f.favoriteServiceIds())
        favServices.append(id.toString(QUuid::WithoutBraces));
    out["favoriteServiceIds"] = favServices;

    QVariantList favProviders;
    for (const auto& id : f.favoriteProviderIds())
        favProviders.append(id.toString(QUuid::WithoutBraces));
    out["favoriteProviderIds"] = favProviders;

    QVariantList history;
    for (const auto& id : f.viewedServiceIds())
        history.append(id.toString(QUuid::WithoutBraces));
    out["viewedServiceIds"] = history;

    out["info"] = f.getInfo();
    out["fullInfo"] = f.getFullInfo();
    return out;
}

bool DataManager::toggleFavoriteService(const QString& serviceId)
{
    if (!m_loggedIn) return false;

    const QUuid sid(serviceId.trimmed());
    if (sid.isNull()) return false;

    Favorites& f = ensureFavoritesForUser(m_favorites, m_currentUser.id);
    f.toggleFavoriteService(sid);

    saveFavorites();
    emit favoritesChanged();
    return true;
}

bool DataManager::toggleFavoriteProvider(const QString& providerId)
{
    if (!m_loggedIn) return false;

    const QUuid pid(providerId.trimmed());
    if (pid.isNull()) return false;

    Favorites& f = ensureFavoritesForUser(m_favorites, m_currentUser.id);
    f.toggleFavoriteProvider(pid);

    saveFavorites();
    emit favoritesChanged();
    return true;
}

bool DataManager::addViewedService(const QString& serviceId)
{
    if (!m_loggedIn) return false;

    const QUuid sid(serviceId.trimmed());
    if (sid.isNull()) return false;

    Favorites& f = ensureFavoritesForUser(m_favorites, m_currentUser.id);
    f.addViewedService(sid, 50);

    saveFavorites();
    emit favoritesChanged();
    return true;
}

bool DataManager::clearMyViewHistory()
{
    if (!m_loggedIn) return false;

    Favorites& f = ensureFavoritesForUser(m_favorites, m_currentUser.id);
    f.clearViewHistory();

    saveFavorites();
    emit favoritesChanged();
    return true;
}
