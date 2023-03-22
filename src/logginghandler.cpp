#include "logginghandler.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include <QSqlError>
#include <QJsonObject>

LoggingHandler::LoggingHandler(QObject *parent) : QObject(parent)
{
}

void LoggingHandler::log(const QString &message, const LogLevel &logLevel) const
{
    if (!settings->enableLogging()) {
        return;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO logs (message, datetime, level) VALUES (:message, :datetime, :logLevel)");
    query.bindValue(":message", message);
    query.bindValue(":logLevel", logLevel);
    query.bindValue(":datetime", QDateTime::currentDateTime().toString(dateTimeFormat));
    query.exec();
}

void LoggingHandler::debug(const QString &message) const
{
    log(message, LogLevel::Debug);
}

void LoggingHandler::warning(const QString &message) const
{
    log(message, LogLevel::Warning);
}

void LoggingHandler::error(const QString &message) const
{
    log(message, LogLevel::Error);
}

void LoggingHandler::deleteLogs() const
{
    QSqlQuery("DELETE FROM logs", database);
}

QJsonArray LoggingHandler::getLogs()
{
    QSqlQuery query("SELECT * FROM logs ORDER BY datetime ASC");
    if (query.lastError().isValid()) {
        emit errorGettingLogs(query.lastError().text());
        return QJsonArray();
    }

    QJsonArray result;

    while(query.next()) {
        QJsonObject log;
        log.insert("datetime", query.value("datetime").toString());
        log.insert("message", query.value("message").toString());
        log.insert("logLevel", query.value("level").toInt());
        result.append(log);
    }

    return result;
}
