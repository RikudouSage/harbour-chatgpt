#ifndef CHATGPTCLIENT_H
#define CHATGPTCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>

#include "chat.h"
#include "secretshandler.h"
#include "settings.h"
#include "logginghandler.h"

class ChatGptClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatGptClient(QObject *parent = nullptr);

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

#endif // CHATGPTCLIENT_H
