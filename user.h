#ifndef USER_H
#define USER_H

#include <QUuid>
#include <QString>
#include <QJsonObject>

class User
{
public:
    enum Role {
        Client = 0,
        Provider = 1,
        Admin = 2
    };

    User();

    QUuid id;
    QString email;
    QString phone;
    Role role;
    bool verified;

    QString passwordSalt;
    QString passwordHash;
    QString verificationCodeHash;

    // business
    bool isValid() const;
    static QString roleToString(Role r);
    static Role roleFromInt(int v);

    bool setPassword(const QString& plainPassword);
    bool checkPassword(const QString& plainPassword) const;

    QString issueVerificationCode();          // возвращает код (для демо)
    bool verifyAccount(const QString& code);  // применяет к текущему объекту

    bool changePassword(const QString& oldPassword, const QString& newPassword);

    // JSON
    QJsonObject toJson() const;
    static User fromJson(const QJsonObject& obj, bool* ok = 0);

private:
    static QString sha256Hex(const QByteArray& data);
    static QString makeSaltHex();
    static QString hashPassword(const QString& saltHex, const QString& password);
};

#endif // USER_H
