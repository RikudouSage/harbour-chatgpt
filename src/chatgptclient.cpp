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
        const auto statusCode = reply->attribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute).toInt();
        logger->debug("Got response with status code " + QString::number(statusCode));
        emit apiKeyChecked(statusCode == 200, apiKey);
        networkAccessManager->deleteLater();
    });

    logger->debug("Checking whether provided api key is valid");
    networkAccessManager->get(request);
}

void ChatGptClient::postMessage(QObject *chatQObject, const QString &message)
{
    auto chat = static_cast<Chat*>(chatQObject);
    chat->appendMessage(message, ChatMessage::Author::User);
    chat->save();
    logger->debug("Added message '" + message + "' to chat with id " + chat->id().toString());

    constexpr auto url = "https://api.openai.com/v1/chat/completions";

    const auto apiKey = secretsHandler->apiKey();
#ifdef QT_DEBUG
    qDebug() << "Api key: " << secretsHandler->apiKey();
#endif
    if (apiKey.isNull() || apiKey.isEmpty()) {
        logger->warning("API key is empty or null, probably some problems with secrets storage.");
        qWarning() << "API key is empty or null, probably some problems with secrets storage.";
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("Authorization", QString("Bearer " + apiKey).toUtf8());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    QJsonObject body;
    QJsonArray messages;
    body["model"] = "gpt-3.5-turbo";

    for (const auto messageQObject : chat->messages().mid(chat->messages().size() - settings->conversationLength())) {
        const auto message = static_cast<ChatMessage*>(messageQObject);
        QJsonObject messageBody;
        messageBody["role"] = message->author() == ChatMessage::Author::User ? "user" : (message->author() == ChatMessage::Author::ChatGPT ? "assistant" : "system");
        messageBody["content"] = message->message();
        messages.append(messageBody);
    }
    body["messages"] = messages;
    body["temperature"] = settings->temperature();
    body["top_p"] = settings->topPercentage();
    body["presence_penalty"] = settings->presencePenalty();
    body["frequency_penalty"] = settings->frequencyPenalty();
    body["stream"] = true;

    const auto json = QJsonDocument(body).toJson();

    logger->debug("Sending request body: " + QString(json));
#ifdef QT_DEBUG
    qDebug() << body;
#endif

    QNetworkAccessManager *networkAccessManager = new QNetworkAccessManager(this);
    auto reply = networkAccessManager->post(request, json);
    emit messageSent();
    logger->debug("Request has been sent");

    connect(reply, &QNetworkReply::readyRead, [=]() {
        const auto raw = QString(reply->readAll());
#ifdef QT_DEBUG
    qDebug() << raw;
#endif
        logger->debug("Got raw reply: " + raw);
        const auto parts = raw.split("data: ");
        for (auto part : parts) {
            logger->debug("Processing partial reply:" + part);
            if (part.trimmed() == "[DONE]") {
                logger->debug("Server has finished sendimg partial messages, whole message has been sent");
                emit messageFinished();
                return;
            }
            const auto document = QJsonDocument::fromJson(part.toUtf8()).object();
            const auto chunk = document["choices"].toArray()[0].toObject()["delta"].toObject()["content"].toString();

            logger->debug("Chunk was extracted from response: " + chunk);
            emit chunkReceived(chunk);
        }
    });
}
