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
    body["stream"] = true;

#ifdef QT_DEBUG
    qDebug() << body;
#endif

    QNetworkAccessManager *networkAccessManager = new QNetworkAccessManager(this);
    auto reply = networkAccessManager->post(request, QJsonDocument(body).toJson());
    emit messageSent();

    connect(reply, &QNetworkReply::readyRead, [=]() {
        const auto raw = QString(reply->readAll());
        const auto parts = raw.split("data: ");
        for (auto part : parts) {
            part = part.trimmed();
            if (part == "[DONE]") {
                emit messageFinished();
                return;
            }
            const auto document = QJsonDocument::fromJson(part.toUtf8()).object();
            const auto chunk = document["choices"].toArray()[0].toObject()["delta"].toObject()["content"].toString();
            if (!chunk.trimmed().isEmpty()) {
                emit chunkReceived(chunk);
            }
        }
    });
}
