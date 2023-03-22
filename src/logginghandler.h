#ifndef LOGGINGHANDLER_H
#define LOGGINGHANDLER_H

#include <QObject>
#include <QSqlDatabase>
#include <QJsonArray>

#include "settings.h"

class LoggingHandler : public QObject
{
    Q_OBJECT
public:
    enum LogLevel {
        Debug,
        Warning,
        Error
    };
    Q_ENUM(LogLevel);

    explicit LoggingHandler(QObject *parent = nullptr);
    void log(const QString &message, const LogLevel &logLevel) const;
    Q_INVOKABLE void debug(const QString &message) const;
    Q_INVOKABLE void warning(const QString &message) const;
    Q_INVOKABLE void error(const QString &message) const;
    Q_INVOKABLE void deleteLogs() const;

    Q_INVOKABLE QJsonArray getLogs();

signals:
    void errorGettingLogs(const QString &error);

private:
    QSqlDatabase database = QSqlDatabase::database();
    Settings *settings = new Settings(this);
    constexpr static auto dateTimeFormat = "yyyy-MM-ddTHH:mm:ss.zzz";
};

#endif // LOGGINGHANDLER_H
