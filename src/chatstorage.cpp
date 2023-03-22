#include "chatstorage.h"

#include <QSqlQuery>
#include <QUuid>
#include <QDebug>
#include <QSqlError>

#include "uuidhelper.h"

ChatStorage::ChatStorage(QObject *parent) : QObject(parent)
{
    database = QSqlDatabase::database();
}

QList<QObject*> ChatStorage::getChats()
{
    QList<QObject*> chats;

    QSqlQuery query("SELECT * FROM chats ORDER BY createdDate DESC", database);
    if (query.lastError().isValid()) {
        logger->error(query.lastError().text());
        return chats;
    }
    while (query.next()) {
        QUuid id(query.value("id").toString());
        QDateTime createdDate = QDateTime::fromString(query.value("createdDate").toString(), Qt::DateFormat::ISODate);
        QString title = query.value("title").toString();
        chats.append(new Chat(id, createdDate, title, this));
    }

    return chats;
}

QObject *ChatStorage::newChat()
{
    return new Chat(this);
}

void ChatStorage::removeChat(const QUuid &id)
{
    QSqlQuery query(database);
    query.prepare("DELETE FROM chats WHERE id = :id");
    query.bindValue(":id", uuidToString(id));
    if (!query.exec()) {
        logger->error("Failed to delete chat with id: " + uuidToString(id) + ", error: " + query.lastError().text());
        qDebug() << "Failed to delete chat with id: " + uuidToString(id) + ", error: " + query.lastError().text();
        return;
    }

    query.prepare("DELETE FROM chat_messages WHERE chat_id = :id");
    query.bindValue(":id", uuidToString(id));
    if (!query.exec()) {
        logger->error("Failed to delete chat messages with chat id: " + uuidToString(id) + ", error: " + query.lastError().text());
        qDebug() << "Failed to delete chat messages with chat id: " + uuidToString(id) + ", error: " + query.lastError().text();
        return;
    }
}
