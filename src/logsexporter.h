#ifndef LOGSEXPORTER_H
#define LOGSEXPORTER_H

#include <QObject>
#include <QJsonArray>

class LogsExporter : public QObject
{
    Q_OBJECT
public:
    explicit LogsExporter(QObject *parent = nullptr);
    Q_INVOKABLE void exportToFile(const QJsonArray &logs);

signals:
    void errorOpeningLogFile(const QString &filename, const QString &basename);
    void exportFinished(const QString &filename, const QString &basename);
};

#endif // LOGSEXPORTER_H
