#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QJsonValue>

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int conversationLength READ conversationLength WRITE setConversationLength NOTIFY conversationLengthChanged)
public:
    explicit Settings(QObject *parent = nullptr);

    int conversationLength();
    void setConversationLength(int length);

signals:
    void conversationLengthChanged();

private:
    void setSetting(const QString &name, QJsonValue value);
    void setSetting(const QString &name, int value);

    QJsonValue getSetting(const QString &name, QJsonValue defaultValue = QJsonValue());
    QString getStringSetting(const QString &name, const QString &defaultValue = "");
    int getIntSetting(const QString &name, const int &defaultValue = 0);

    QString jsonValue(QJsonValue value);
    QJsonValue toJsonValue(const QString &value);

private:
    QSqlDatabase database = QSqlDatabase::database();

};

#endif // SETTINGS_H
