#ifndef LOGGINGHANDLER_H
#define LOGGINGHANDLER_H

#include <QObject>
#include <QSqlDatabase>

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

private:
    QSqlDatabase database = QSqlDatabase::database();
    Settings *settings = new Settings(this);
};

#endif // LOGGINGHANDLER_H
