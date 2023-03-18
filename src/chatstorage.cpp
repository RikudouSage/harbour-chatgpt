#include "chatstorage.h"

#include <QSqlQuery>
#include <QUuid>
#include <QDebug>

#include "uuidhelper.h"

ChatStorage::ChatStorage(QObject *parent) : QObject(parent)
{
    database = QSqlDatabase::database();
}

QList<QObject*> ChatStorage::getChats()
{
    QList<QObject*> chats;

    QSqlQuery query("SELECT * FROM chats ORDER BY createdDate DESC", database);
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
        qDebug() << "Failed to delete chat with id: " << uuidToString(id);
    }

    query.prepare("DELETE FROM chat_messages WHERE chat_id = :id");
    query.bindValue(":id", uuidToString(id));
    if (!query.exec()) {
        qDebug() << "Failed to delete chat messages with chat id: " << uuidToString(id);
    }
}
