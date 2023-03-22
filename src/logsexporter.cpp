#include "logsexporter.h"

#include <QStandardPaths>
#include <QJsonObject>
#include <QDateTime>

#include <QFile>
#include <QTextStream>

LogsExporter::LogsExporter(QObject *parent) : QObject(parent)
{

}

void LogsExporter::exportToFile(const QJsonArray &logs)
{
    const auto now = QDateTime::currentDateTime().toString(Qt::DateFormat::ISODate).replace(":", "-");
    const auto basename = "ChatGPT." + now + ".log";
    const auto filename = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + basename;
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        emit errorOpeningLogFile(filename, basename);
        return;
    }
    QTextStream stream(&file);
    for (const auto log : logs) {
        const auto logObject = log.toObject();
        stream << "[" << logObject.value("datetime").toString() << "] " << logObject.value("message").toString() << "\n";
    }
    file.close();

    emit exportFinished(filename, basename);
}
