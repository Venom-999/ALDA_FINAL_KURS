#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QVariantList>
#include <QVariantMap>
#include <QUuid>
#include <QDateTime>

#include "catalog.h"
#include "profile.h"
#include "request.h"
#include "subscription.h"
#include "favorites.h"
#include "review.h"

class DataManager : public QObject
{
    Q_OBJECT

    // --- QML visible state ---
    Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(QString currentUserId READ currentUserId NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserEmail READ currentUserEmail NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserPhone READ currentUserPhone NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserRole READ currentUserRole NOTIFY currentUserChanged)
    Q_PROPERTY(bool currentUserVerified READ currentUserVerified NOTIFY currentUserChanged)

public:
    explicit DataManager(QObject* parent = nullptr);
    static DataManager& instance();

    // --- properties getters ---
    bool loggedIn() const { return m_loggedIn; }
    QString currentUserId() const { return m_currentUser.id.toString(QUuid::WithoutBraces); }
    QString currentUserEmail() const { return m_currentUser.email; }
    QString currentUserPhone() const { return m_currentUser.phone; }
    QString currentUserRole() const;
    bool currentUserVerified() const { return m_currentUser.verified; }

    // ---------------- Auth/User ----------------
    Q_INVOKABLE bool registerUser(const QString& email,
                                  const QString& phone,
                                  int roleIndex,
                                  const QString& password);

    Q_INVOKABLE bool login(const QString& emailOrPhone, const QString& password);
    Q_INVOKABLE void logout();

    Q_INVOKABLE QString issueVerificationCode(); // demo code (6 digits)
    Q_INVOKABLE bool verifyAccount(const QString& code);
    Q_INVOKABLE bool changePassword(const QString& oldPass, const QString& newPass);

    // ---------------- Profile ----------------
    Q_INVOKABLE QVariantMap getMyProfile() const;

    Q_INVOKABLE bool saveMyProfile(const QString& name,
                                   const QString& description,
                                   const QString& avatarPath,
                                   const QString& contactEmail,
                                   const QString& contactPhone);

    // ---------------- Services/Catalog ----------------
    Q_INVOKABLE QVariantList getAllServices() const;

    Q_INVOKABLE bool addService(const QVariantMap& serviceMap);
    Q_INVOKABLE bool updateService(const QVariantMap& serviceMap);
    Q_INVOKABLE bool deleteService(const QString& serviceId);

    // Catalog wrappers
    Q_INVOKABLE QVariantList catalogGetActiveServices() const;
    Q_INVOKABLE QVariantList catalogSearchByName(const QString& name);
    Q_INVOKABLE QVariantList catalogSearchByDescription(const QString& text);
    Q_INVOKABLE QVariantList catalogFilterByCategory(const QString& category) const;
    Q_INVOKABLE QVariantList catalogFilterByPrice(double minPrice, double maxPrice) const;
    Q_INVOKABLE QVariantList catalogFilterByRating(double minRating) const;
    Q_INVOKABLE QVariantList catalogGetPopularServices(int count) const;
    Q_INVOKABLE QVariantList catalogGetNewServices(int count) const;
    Q_INVOKABLE QStringList catalogGetCategories() const;
    Q_INVOKABLE QStringList catalogGetSearchHistory() const;
    Q_INVOKABLE QString catalogGetInfo() const;
    Q_INVOKABLE QString catalogGetFullInfo() const;

    // ---------------- Requests ----------------
    Q_INVOKABLE QVariantList getAllRequests() const;

    Q_INVOKABLE QString createRequest(const QString& serviceId,
                                      const QString& providerId,
                                      const QString& description);

    Q_INVOKABLE bool deleteRequest(const QString& requestId);
    Q_INVOKABLE bool updateRequestStatus(const QString& requestId, int statusIndex);
    Q_INVOKABLE bool updateRequestDescription(const QString& requestId, const QString& description);
    Q_INVOKABLE bool addRequestComment(const QString& requestId, const QString& comment);

    // ---------------- Reviews ----------------
    Q_INVOKABLE QVariantList getReviewsForService(const QString& serviceId) const;
    Q_INVOKABLE bool addReview(const QString& serviceId, int rating, const QString& comment);

    // ---------------- Subscription ----------------
    Q_INVOKABLE QVariantMap getMySubscription() const;

    Q_INVOKABLE bool saveMySubscription(const QString& planType,
                                        double price,
                                        const QString& startIso,
                                        const QString& endIso,
                                        bool active);

    Q_INVOKABLE bool cancelMySubscription();

    // ---------------- Favorites ----------------
    Q_INVOKABLE QVariantMap getMyFavorites() const;
    Q_INVOKABLE bool toggleFavoriteService(const QString& serviceId);
    Q_INVOKABLE bool toggleFavoriteProvider(const QString& providerId);
    Q_INVOKABLE bool addViewedService(const QString& serviceId);
    Q_INVOKABLE bool clearMyViewHistory();

signals:
    void loggedInChanged();
    void currentUserChanged();

    void servicesChanged();
    void requestsChanged();
    void reviewsChanged();
    void subscriptionsChanged();
    void favoritesChanged();

private:
    // ---- storage helpers ----
    void loadServices();
    void saveServices() const;

    void loadRequests();
    void saveRequests() const;

    void loadReviews();
    void saveReviews() const;

    void loadSubscriptions();
    void saveSubscriptions() const;

    void loadFavorites();
    void saveFavorites() const;

    // ---- conversion helpers ----
    static QVariantList servicesToVariantList(const QVector<Service>& v);
    static QVariantList requestsToVariantList(const QVector<Request>& v);

    int indexOfRequest(const QUuid& id) const;
    int findUserByEmailOrPhone(const QString& s) const;

    static QString hashPasswordHex(const QString& pass);
    static QString genCode6();

private:
    struct UserRec {
        QUuid id;
        QString email;
        QString phone;
        int roleIndex = 0; // 0=Client,1=Provider,2=Admin
        QString passHashHex;
        QDateTime createdAt;
        bool verified = false;
        QString verificationCode;
    };

private:
    bool m_loggedIn = false;
    UserRec m_currentUser;

    QVector<UserRec> m_users;      // demo users (in-memory)
    QVector<Profile> m_profiles;   // demo profiles (in-memory)

    Catalog m_catalog;

    QVector<Request> m_requests;
    QVector<Review> m_reviews;
    QVector<Subscription> m_subscriptions;
    QVector<Favorites> m_favorites;
};

#endif // DATAMANAGER_H
