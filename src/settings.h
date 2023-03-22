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
    Q_PROPERTY(double temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(double topPercentage READ topPercentage WRITE setTopPercentage NOTIFY topPercentageChanged)
    Q_PROPERTY(double presencePenalty READ presencePenalty WRITE setPresencePenalty NOTIFY presencePenaltyChanged)
    Q_PROPERTY(double frequencyPenalty READ frequencyPenalty WRITE setFrequencyPenalty NOTIFY frequencyPenaltyChanged)
    Q_PROPERTY(bool enableLogging READ enableLogging WRITE setEnableLogging NOTIFY enableLoggingChanged)
    Q_PROPERTY(QString systemMessage READ systemMessage WRITE setSystemMessage NOTIFY systemMessageChanged)
public:
    explicit Settings(QObject *parent = nullptr);

    int conversationLength();
    void setConversationLength(int length);
    double temperature();
    void setTemperature(double temperature);
    double topPercentage();
    void setTopPercentage(double percentage);
    double presencePenalty();
    void setPresencePenalty(double penalty);
    double frequencyPenalty();
    void setFrequencyPenalty(double penalty);
    bool enableLogging();
    void setEnableLogging(bool enable);
    QString systemMessage();
    void setSystemMessage(const QString &message);

signals:
    void conversationLengthChanged();
    void temperatureChanged();
    void topPercentageChanged();
    void presencePenaltyChanged();
    void frequencyPenaltyChanged();
    void enableLoggingChanged();
    void systemMessageChanged();

private:
    void setSetting(const QString &name, QJsonValue value);
    void setSetting(const QString &name, int value);
    void setSetting(const QString &name, double value);
    void setSetting(const QString &name, bool value);

    QJsonValue getSetting(const QString &name, QJsonValue defaultValue = QJsonValue());
    QString getStringSetting(const QString &name, const QString &defaultValue = "");
    int getIntSetting(const QString &name, const int &defaultValue = 0);
    double getDoubleSetting(const QString &name, const double &defaultValue = 0);
    bool getBoolSetting(const QString &name, const bool &defaultValue = false);

    QString jsonValue(QJsonValue value);
    QJsonValue toJsonValue(const QString &value);

private:
    QSqlDatabase database = QSqlDatabase::database();

};

#endif // SETTINGS_H
