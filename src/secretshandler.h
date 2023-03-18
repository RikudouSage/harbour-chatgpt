#ifndef SECRETSHANDLER_H
#define SECRETSHANDLER_H

#include <QObject>

#include <Sailfish/Secrets/secretmanager.h>
#include <Sailfish/Secrets/request.h>
#include <Sailfish/Secrets/secret.h>

using Sailfish::Secrets::SecretManager;
using Sailfish::Secrets::Request;
using Sailfish::Secrets::Secret;

class SecretsHandler : public QObject
{
    Q_OBJECT
public:
    explicit SecretsHandler(QObject *parent = nullptr);

    QString apiKey();
    Q_INVOKABLE bool hasApiKey();
    Q_INVOKABLE void setApiKey(const QString &apiKey);
    void removeApiKey();

private:
    static const QString collectionName;
    SecretManager* secretManager = new SecretManager(this);
    bool hasCollection = false;

    bool isResultValid(const Request &request);
    bool isSecretValid(const Secret &secret);
    bool storeData(const QString &name, const QString &data);
    Secret getSecret(const QString &name);
    bool deleteSecret(const QString &name);
    QString getData(const QString &name);
    bool createCollection();
    Secret::Identifier toIdentifier(const QString &name);
};

#endif // SECRETSHANDLER_H
