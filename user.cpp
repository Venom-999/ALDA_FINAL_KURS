#include "user.h"

#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDateTime>

User::User()
    : id(QUuid::createUuid())
    , role(Client)
    , verified(false)
{
}

bool User::isValid() const
{
    return !id.isNull() && !email.trimmed().isEmpty();
}

QString User::roleToString(Role r)
{
    switch (r) {
    case Client: return "Client";
    case Provider: return "Provider";
    case Admin: return "Admin";
    }
    return "Unknown";
}

User::Role User::roleFromInt(int v)
{
    if (v == 1) return Provider;
    if (v == 2) return Admin;
    return Client;
}

// -------- crypto helpers --------

QString User::sha256Hex(const QByteArray& data)
{
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex());
}

QString User::makeSaltHex()
{
    QByteArray bytes(16, Qt::Uninitialized);
    for (int i = 0; i < bytes.size(); ++i)
        bytes[i] = static_cast<char>(QRandomGenerator::global()->generate() & 0xFF);
    return QString(bytes.toHex());
}

QString User::hashPassword(const QString& saltHex, const QString& password)
{
    return sha256Hex((saltHex + ":" + password).toUtf8());
}

bool User::setPassword(const QString& plainPassword)
{
    if (plainPassword.length() < 6)
        return false;

    passwordSalt = makeSaltHex();
    passwordHash = hashPassword(passwordSalt, plainPassword);
    return true;
}

bool User::checkPassword(const QString& plainPassword) const
{
    if (passwordSalt.isEmpty() || passwordHash.isEmpty())
        return false;
    return hashPassword(passwordSalt, plainPassword) == passwordHash;
}

// -------- verification --------

QString User::issueVerificationCode()
{
    const int code = static_cast<int>(QRandomGenerator::global()->bounded(100000, 1000000)); // 6 digits
    const QString codeStr = QString::number(code);

    verificationCodeHash = sha256Hex(codeStr.toUtf8());
    verified = false;

    return codeStr; // для демо показываем пользователю
}

bool User::verifyAccount(const QString& code)
{
    if (code.trimmed().isEmpty())
        return false;

    // если код не выдавали — отклоняем
    if (verificationCodeHash.isEmpty())
        return false;

    if (sha256Hex(code.toUtf8()) != verificationCodeHash)
        return false;

    verified = true;
    verificationCodeHash.clear();
    return true;
}

bool User::changePassword(const QString& oldPassword, const QString& newPassword)
{
    if (!checkPassword(oldPassword))
        return false;
    return setPassword(newPassword);
}

// -------- JSON --------

QJsonObject User::toJson() const
{
    QJsonObject o;
    o["id"] = id.toString(QUuid::WithoutBraces);
    o["email"] = email;
    o["phone"] = phone;
    o["role"] = static_cast<int>(role);
    o["verified"] = verified;
    o["salt"] = passwordSalt;
    o["passwordHash"] = passwordHash;
    o["verificationCodeHash"] = verificationCodeHash;
    return o;
}

User User::fromJson(const QJsonObject& obj, bool* ok)
{
    User u;

    const QString idStr = obj.value("id").toString();
    const QString em = obj.value("email").toString();

    if (idStr.isEmpty() || em.isEmpty()) {
        if (ok) *ok = false;
        return User();
    }

    u.id = QUuid(idStr);
    u.email = em;
    u.phone = obj.value("phone").toString();
    u.role = roleFromInt(obj.value("role").toInt(0));
    u.verified = obj.value("verified").toBool(false);

    u.passwordSalt = obj.value("salt").toString();
    u.passwordHash = obj.value("passwordHash").toString();
    u.verificationCodeHash = obj.value("verificationCodeHash").toString();

    if (ok) *ok = true;
    return u;
}
