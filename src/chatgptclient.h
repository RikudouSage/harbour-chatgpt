#ifndef CHATGPTCLIENT_H
#define CHATGPTCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>

#include "chat.h"
#include "secretshandler.h"

class ChatGptClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatGptClient(QObject *parent = nullptr);

    Q_INVOKABLE void checkApiKey(const QString &apiKey);
    Q_INVOKABLE void postMessage(QObject *chat, const QString &message);

signals:
    void apiKeyChecked(bool isValid, const QString &apiKey);
    void unauthorized();
    void error(const QString &reason);
private:
    SecretsHandler *secretsHandler = new SecretsHandler(this);
};

#endif // CHATGPTCLIENT_H
