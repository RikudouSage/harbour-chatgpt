#ifndef OPENAICLIENT_H
#define OPENAICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>

#include "chat.h"
#include "secretshandler.h"
#include "settings.h"
#include "logginghandler.h"

class OpenAiClient : public QObject
{
    Q_OBJECT
public:
    explicit OpenAiClient(QObject *parent = nullptr);

    Q_INVOKABLE void checkApiKey(const QString &apiKey);
    Q_INVOKABLE void postMessage(QObject *chat, const QString &message);
    Q_INVOKABLE void getModels();

signals:
    void apiKeyChecked(bool isValid, const QString &apiKey);
    void modelsListReceived(const QStringList &models);
    void unauthorized();
    void chunkReceived(const QString &chunk);
    void messageFinished();
    void messageSent();

private:
    void listModels(const QString &apiKey, bool emitApiKeyCheck, bool emitModelsList);

private:
    SecretsHandler *secretsHandler = new SecretsHandler(this);
    Settings *settings = new Settings(this);
    const LoggingHandler *logger = new LoggingHandler(this);
};

#endif // OPENAICLIENT_H
