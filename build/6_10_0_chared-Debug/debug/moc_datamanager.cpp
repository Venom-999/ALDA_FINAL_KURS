/****************************************************************************
** Meta object code from reading C++ file 'datamanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../datamanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datamanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11DataManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto DataManager::qt_create_metaobjectdata<qt_meta_tag_ZN11DataManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DataManager",
        "loggedInChanged",
        "",
        "currentUserChanged",
        "servicesChanged",
        "requestsChanged",
        "reviewsChanged",
        "subscriptionsChanged",
        "favoritesChanged",
        "registerUser",
        "email",
        "phone",
        "roleIndex",
        "password",
        "login",
        "emailOrPhone",
        "logout",
        "issueVerificationCode",
        "verifyAccount",
        "code",
        "changePassword",
        "oldPass",
        "newPass",
        "getMyProfile",
        "QVariantMap",
        "saveMyProfile",
        "name",
        "description",
        "avatarPath",
        "contactEmail",
        "contactPhone",
        "getAllServices",
        "QVariantList",
        "addService",
        "serviceMap",
        "updateService",
        "deleteService",
        "serviceId",
        "catalogGetActiveServices",
        "catalogSearchByName",
        "catalogSearchByDescription",
        "text",
        "catalogFilterByCategory",
        "category",
        "catalogFilterByPrice",
        "minPrice",
        "maxPrice",
        "catalogFilterByRating",
        "minRating",
        "catalogGetPopularServices",
        "count",
        "catalogGetNewServices",
        "catalogGetCategories",
        "catalogGetSearchHistory",
        "catalogGetInfo",
        "catalogGetFullInfo",
        "getAllRequests",
        "createRequest",
        "providerId",
        "deleteRequest",
        "requestId",
        "updateRequestStatus",
        "statusIndex",
        "updateRequestDescription",
        "addRequestComment",
        "comment",
        "getReviewsForService",
        "addReview",
        "rating",
        "getMySubscription",
        "saveMySubscription",
        "planType",
        "price",
        "startIso",
        "endIso",
        "active",
        "cancelMySubscription",
        "getMyFavorites",
        "toggleFavoriteService",
        "toggleFavoriteProvider",
        "addViewedService",
        "clearMyViewHistory",
        "loggedIn",
        "currentUserId",
        "currentUserEmail",
        "currentUserPhone",
        "currentUserRole",
        "currentUserVerified"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'loggedInChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'currentUserChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'servicesChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'requestsChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'reviewsChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'subscriptionsChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'favoritesChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'registerUser'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, int, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 10 }, { QMetaType::QString, 11 }, { QMetaType::Int, 12 }, { QMetaType::QString, 13 },
        }}),
        // Method 'login'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 15 }, { QMetaType::QString, 13 },
        }}),
        // Method 'logout'
        QtMocHelpers::MethodData<void()>(16, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'issueVerificationCode'
        QtMocHelpers::MethodData<QString()>(17, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'verifyAccount'
        QtMocHelpers::MethodData<bool(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 19 },
        }}),
        // Method 'changePassword'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 21 }, { QMetaType::QString, 22 },
        }}),
        // Method 'getMyProfile'
        QtMocHelpers::MethodData<QVariantMap() const>(23, 2, QMC::AccessPublic, 0x80000000 | 24),
        // Method 'saveMyProfile'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &, const QString &)>(25, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 26 }, { QMetaType::QString, 27 }, { QMetaType::QString, 28 }, { QMetaType::QString, 29 },
            { QMetaType::QString, 30 },
        }}),
        // Method 'getAllServices'
        QtMocHelpers::MethodData<QVariantList() const>(31, 2, QMC::AccessPublic, 0x80000000 | 32),
        // Method 'addService'
        QtMocHelpers::MethodData<bool(const QVariantMap &)>(33, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 24, 34 },
        }}),
        // Method 'updateService'
        QtMocHelpers::MethodData<bool(const QVariantMap &)>(35, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { 0x80000000 | 24, 34 },
        }}),
        // Method 'deleteService'
        QtMocHelpers::MethodData<bool(const QString &)>(36, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 37 },
        }}),
        // Method 'catalogGetActiveServices'
        QtMocHelpers::MethodData<QVariantList() const>(38, 2, QMC::AccessPublic, 0x80000000 | 32),
        // Method 'catalogSearchByName'
        QtMocHelpers::MethodData<QVariantList(const QString &)>(39, 2, QMC::AccessPublic, 0x80000000 | 32, {{
            { QMetaType::QString, 26 },
        }}),
        // Method 'catalogSearchByDescription'
        QtMocHelpers::MethodData<QVariantList(const QString &)>(40, 2, QMC::AccessPublic, 0x80000000 | 32, {{
            { QMetaType::QString, 41 },
        }}),
        // Method 'catalogFilterByCategory'
        QtMocHelpers::MethodData<QVariantList(const QString &) const>(42, 2, QMC::AccessPublic, 0x80000000 | 32, {{
            { QMetaType::QString, 43 },
        }}),
        // Method 'catalogFilterByPrice'
        QtMocHelpers::MethodData<QVariantList(double, double) const>(44, 2, QMC::AccessPublic, 0x80000000 | 32, {{
            { QMetaType::Double, 45 }, { QMetaType::Double, 46 },
        }}),
        // Method 'catalogFilterByRating'
        QtMocHelpers::MethodData<QVariantList(double) const>(47, 2, QMC::AccessPublic, 0x80000000 | 32, {{
            { QMetaType::Double, 48 },
        }}),
        // Method 'catalogGetPopularServices'
        QtMocHelpers::MethodData<QVariantList(int) const>(49, 2, QMC::AccessPublic, 0x80000000 | 32, {{
            { QMetaType::Int, 50 },
        }}),
        // Method 'catalogGetNewServices'
        QtMocHelpers::MethodData<QVariantList(int) const>(51, 2, QMC::AccessPublic, 0x80000000 | 32, {{
            { QMetaType::Int, 50 },
        }}),
        // Method 'catalogGetCategories'
        QtMocHelpers::MethodData<QStringList() const>(52, 2, QMC::AccessPublic, QMetaType::QStringList),
        // Method 'catalogGetSearchHistory'
        QtMocHelpers::MethodData<QStringList() const>(53, 2, QMC::AccessPublic, QMetaType::QStringList),
        // Method 'catalogGetInfo'
        QtMocHelpers::MethodData<QString() const>(54, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'catalogGetFullInfo'
        QtMocHelpers::MethodData<QString() const>(55, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'getAllRequests'
        QtMocHelpers::MethodData<QVariantList() const>(56, 2, QMC::AccessPublic, 0x80000000 | 32),
        // Method 'createRequest'
        QtMocHelpers::MethodData<QString(const QString &, const QString &, const QString &)>(57, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 37 }, { QMetaType::QString, 58 }, { QMetaType::QString, 27 },
        }}),
        // Method 'deleteRequest'
        QtMocHelpers::MethodData<bool(const QString &)>(59, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 60 },
        }}),
        // Method 'updateRequestStatus'
        QtMocHelpers::MethodData<bool(const QString &, int)>(61, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 60 }, { QMetaType::Int, 62 },
        }}),
        // Method 'updateRequestDescription'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(63, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 60 }, { QMetaType::QString, 27 },
        }}),
        // Method 'addRequestComment'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(64, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 60 }, { QMetaType::QString, 65 },
        }}),
        // Method 'getReviewsForService'
        QtMocHelpers::MethodData<QVariantList(const QString &) const>(66, 2, QMC::AccessPublic, 0x80000000 | 32, {{
            { QMetaType::QString, 37 },
        }}),
        // Method 'addReview'
        QtMocHelpers::MethodData<bool(const QString &, int, const QString &)>(67, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 37 }, { QMetaType::Int, 68 }, { QMetaType::QString, 65 },
        }}),
        // Method 'getMySubscription'
        QtMocHelpers::MethodData<QVariantMap() const>(69, 2, QMC::AccessPublic, 0x80000000 | 24),
        // Method 'saveMySubscription'
        QtMocHelpers::MethodData<bool(const QString &, double, const QString &, const QString &, bool)>(70, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 71 }, { QMetaType::Double, 72 }, { QMetaType::QString, 73 }, { QMetaType::QString, 74 },
            { QMetaType::Bool, 75 },
        }}),
        // Method 'cancelMySubscription'
        QtMocHelpers::MethodData<bool()>(76, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'getMyFavorites'
        QtMocHelpers::MethodData<QVariantMap() const>(77, 2, QMC::AccessPublic, 0x80000000 | 24),
        // Method 'toggleFavoriteService'
        QtMocHelpers::MethodData<bool(const QString &)>(78, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 37 },
        }}),
        // Method 'toggleFavoriteProvider'
        QtMocHelpers::MethodData<bool(const QString &)>(79, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 58 },
        }}),
        // Method 'addViewedService'
        QtMocHelpers::MethodData<bool(const QString &)>(80, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 37 },
        }}),
        // Method 'clearMyViewHistory'
        QtMocHelpers::MethodData<bool()>(81, 2, QMC::AccessPublic, QMetaType::Bool),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'loggedIn'
        QtMocHelpers::PropertyData<bool>(82, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
        // property 'currentUserId'
        QtMocHelpers::PropertyData<QString>(83, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'currentUserEmail'
        QtMocHelpers::PropertyData<QString>(84, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'currentUserPhone'
        QtMocHelpers::PropertyData<QString>(85, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'currentUserRole'
        QtMocHelpers::PropertyData<QString>(86, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'currentUserVerified'
        QtMocHelpers::PropertyData<bool>(87, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DataManager, qt_meta_tag_ZN11DataManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DataManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11DataManagerE_t>.metaTypes,
    nullptr
} };

void DataManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DataManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->loggedInChanged(); break;
        case 1: _t->currentUserChanged(); break;
        case 2: _t->servicesChanged(); break;
        case 3: _t->requestsChanged(); break;
        case 4: _t->reviewsChanged(); break;
        case 5: _t->subscriptionsChanged(); break;
        case 6: _t->favoritesChanged(); break;
        case 7: { bool _r = _t->registerUser((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 8: { bool _r = _t->login((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 9: _t->logout(); break;
        case 10: { QString _r = _t->issueVerificationCode();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->verifyAccount((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 12: { bool _r = _t->changePassword((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 13: { QVariantMap _r = _t->getMyProfile();
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 14: { bool _r = _t->saveMyProfile((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 15: { QVariantList _r = _t->getAllServices();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 16: { bool _r = _t->addService((*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->updateService((*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { bool _r = _t->deleteService((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 19: { QVariantList _r = _t->catalogGetActiveServices();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 20: { QVariantList _r = _t->catalogSearchByName((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 21: { QVariantList _r = _t->catalogSearchByDescription((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 22: { QVariantList _r = _t->catalogFilterByCategory((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 23: { QVariantList _r = _t->catalogFilterByPrice((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 24: { QVariantList _r = _t->catalogFilterByRating((*reinterpret_cast<std::add_pointer_t<double>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 25: { QVariantList _r = _t->catalogGetPopularServices((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 26: { QVariantList _r = _t->catalogGetNewServices((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 27: { QStringList _r = _t->catalogGetCategories();
            if (_a[0]) *reinterpret_cast<QStringList*>(_a[0]) = std::move(_r); }  break;
        case 28: { QStringList _r = _t->catalogGetSearchHistory();
            if (_a[0]) *reinterpret_cast<QStringList*>(_a[0]) = std::move(_r); }  break;
        case 29: { QString _r = _t->catalogGetInfo();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 30: { QString _r = _t->catalogGetFullInfo();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 31: { QVariantList _r = _t->getAllRequests();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 32: { QString _r = _t->createRequest((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 33: { bool _r = _t->deleteRequest((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 34: { bool _r = _t->updateRequestStatus((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 35: { bool _r = _t->updateRequestDescription((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 36: { bool _r = _t->addRequestComment((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 37: { QVariantList _r = _t->getReviewsForService((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        case 38: { bool _r = _t->addReview((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 39: { QVariantMap _r = _t->getMySubscription();
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 40: { bool _r = _t->saveMySubscription((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[5])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 41: { bool _r = _t->cancelMySubscription();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 42: { QVariantMap _r = _t->getMyFavorites();
            if (_a[0]) *reinterpret_cast<QVariantMap*>(_a[0]) = std::move(_r); }  break;
        case 43: { bool _r = _t->toggleFavoriteService((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 44: { bool _r = _t->toggleFavoriteProvider((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 45: { bool _r = _t->addViewedService((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 46: { bool _r = _t->clearMyViewHistory();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::loggedInChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::currentUserChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::servicesChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::requestsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::reviewsChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::subscriptionsChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::favoritesChanged, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->loggedIn(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->currentUserId(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->currentUserEmail(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->currentUserPhone(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->currentUserRole(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->currentUserVerified(); break;
        default: break;
        }
    }
}

const QMetaObject *DataManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 47)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 47;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 47)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 47;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void DataManager::loggedInChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DataManager::currentUserChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DataManager::servicesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DataManager::requestsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DataManager::reviewsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void DataManager::subscriptionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void DataManager::favoritesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
