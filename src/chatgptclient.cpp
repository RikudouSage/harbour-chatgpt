#include "chatgptclient.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <QDebug>

ChatGptClient::ChatGptClient(QObject *parent) : QObject(parent)
{

}

void ChatGptClient::checkApiKey(const QString &apiKey)
{
    constexpr auto url = "https://api.openai.com/v1/models";

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("Authorization", QString("Bearer " + apiKey).toUtf8());

    QNetworkAccessManager *networkAccessManager = new QNetworkAccessManager(this);

    connect(networkAccessManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        emit apiKeyChecked(reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt() == 200, apiKey);
        networkAccessManager->deleteLater();
    });

    networkAccessManager->get(request);
}

void ChatGptClient::postMessage(QObject *chatQObject, const QString &message)
{
    auto chat = static_cast<Chat*>(chatQObject);
    chat->appendMessage(message, ChatMessage::Author::User);
    chat->save();

    constexpr auto url = "https://api.openai.com/v1/chat/completions";

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("Authorization", QString("Bearer " + secretsHandler->apiKey()).toUtf8());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

#ifdef QT_DEBUG
   qDebug() << "Api key: " << secretsHandler->apiKey();
#endif

    QJsonObject body;
    QJsonArray messages;
    body["model"] = "gpt-3.5-turbo";

    for (const auto messageQObject : chat->messages().mid(chat->messages().size() - settings->conversationLength())) {
        const auto message = static_cast<ChatMessage*>(messageQObject);
        QJsonObject messageBody;
        messageBody["role"] = message->author() == ChatMessage::Author::User ? "user" : "assistant";
        messageBody["content"] = message->message();
        messages.append(messageBody);
    }
    body["messages"] = messages;
    body["temperature"] = settings->temperature();
    body["top_p"] = settings->topPercentage();
    body["presence_penalty"] = settings->presencePenalty();
    body["frequency_penalty"] = settings->frequencyPenalty();

#ifdef QT_DEBUG
    qDebug() << body;
#endif

    QNetworkAccessManager *networkAccessManager = new QNetworkAccessManager(this);
    connect(networkAccessManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        networkAccessManager->deleteLater();

        const auto statusCode = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();
        if (statusCode == 401) {
            emit unauthorized();
            return;
        }

        if (statusCode != 200) {
            emit error(tr("There was a network error."));
            return;
        }

        QJsonObject response = QJsonDocument::fromJson(reply->readAll()).object();
#ifdef QT_DEBUG
        qDebug() << response;
#endif
        const auto choices = response["choices"].toArray();
        if (choices.size() < 1) {
            emit error(tr("ChatGPT provided no response."));
            return;
        }
        const auto message = choices[0].toObject()["message"].toObject()["content"].toString().trimmed();
        chat->appendMessage(message, ChatMessage::Author::ChatGPT);
        chat->save();
    });
    networkAccessManager->post(request, QJsonDocument(body).toJson());
}
