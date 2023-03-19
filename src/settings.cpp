#include "settings.h"

#include <QJsonDocument>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QRegExp>

#include "databasemanager.h"

Settings::Settings(QObject *parent) : QObject(parent)
{

}

int Settings::conversationLength()
{
    return getIntSetting("conversationLength", 10);
}

void Settings::setConversationLength(int length)
{
    if (length != conversationLength()) {
        setSetting("conversationLength", length);
        emit conversationLengthChanged();
    }
}

void Settings::setSetting(const QString &name, QJsonValue value)
{
    QSqlQuery existing(database);
    existing.prepare("SELECT * FROM settings WHERE setting = :setting");
    existing.bindValue(":setting", name);
    if (!existing.exec()) {
        qDebug() << "Failed executing query to get setting " << name;
    }

    QString query;
    if (DatabaseManager::getQuerySize(existing) <= 0) {
        query = "INSERT INTO settings (setting, value) VALUES (:setting, :value)";
    } else {
        query = "UPDATE settings SET value = :value WHERE setting = :setting";
    }

    QSqlQuery saveQuery(database);
    saveQuery.prepare(query);
    saveQuery.bindValue(":setting", name);
    saveQuery.bindValue(":value", jsonValue(value));
    if (!saveQuery.exec()) {
        qDebug() << "Failed to execute query: " << query << " (" << name << ", " << jsonValue(value) << ")" << saveQuery.lastError().text();
    }
}

void Settings::setSetting(const QString &name, int value)
{
    setSetting(name, QJsonValue(value));
}

QJsonValue Settings::getSetting(const QString &name, QJsonValue defaultValue)
{
    QSqlQuery existing(database);
    existing.prepare("SELECT * FROM settings WHERE setting = :setting");
    existing.bindValue(":setting", name);
    if (!existing.exec()) {
        qDebug() << "Failed executing query to get setting " << name;
    }

    if (DatabaseManager::getQuerySize(existing) <= 0) {
        return defaultValue;
    }

    existing.next();
    return toJsonValue(existing.value("value").toString());
}

QString Settings::getStringSetting(const QString &name, const QString &defaultValue)
{
    return getSetting(name, QJsonValue(defaultValue)).toString();
}

int Settings::getIntSetting(const QString &name, const int &defaultValue)
{
    return getSetting(name, QJsonValue(defaultValue)).toInt();
}

QString Settings::jsonValue(QJsonValue value)
{
    if (value.isString()) {
        return "\"" + value.toString() + "\"";
    }
    if (value.isBool()) {
        return value.toBool() ? "true" : "false";
    }
    if (value.isDouble()) {
        return QString::number(value.toDouble());
    }
    if (value.isNull()) {
        return "null";
    }
    if (value.isArray()) {
        QString result = "[";

        for(const auto item : value.toArray()) {
            result += jsonValue(item) + ",";
        }
        result = result.mid(0, result.size() - 1);

        result += "]";

        return result;
    }

    return nullptr;
}

QJsonValue Settings::toJsonValue(const QString &value)
{
    if (value.size() >= 2 && value.at(0) == QString("\"") && value.at(value.size() - 1) == QString("\"")) {
        return QJsonValue(value.mid(1, value.size() - 1));
    }
    if (QRegExp("[0-9]+").exactMatch(value)) {
        return QJsonValue(value.toInt());
    }
    if (QRegExp("[0-9.]+").exactMatch(value)) {
        return QJsonValue(value.toDouble());
    }
    if (value == "null") {
        return QJsonValue();
    }
    if (value == "true") {
        return QJsonValue(true);
    }
    if (value == "false") {
        return QJsonValue(false);
    }

    return QJsonValue(QJsonValue::Type::Undefined);
}