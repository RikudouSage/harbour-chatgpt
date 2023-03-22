#include "chatmessage.h"

#include <QVariant>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include "uuidhelper.h"

ChatMessage::ChatMessage(QDateTime dateTime, Author author, QString message, QObject *parent):
    QObject(parent),
    m_dateTime(dateTime),
    m_author(author),
    m_message(message),
    m_id(QUuid::createUuid()),
    m_isNew(true)
{
}

ChatMessage::ChatMessage(QUuid id, QDateTime dateTime, Author author, QString message, QObject *parent):
    QObject(parent),
    m_dateTime(dateTime),
    m_author(author),
    m_message(message),
    m_id(id),
    m_isNew(false)
{
}

ChatMessage::Author ChatMessage::author()
{
    return m_author;
}

QString ChatMessage::message()
{
    return m_message;
}

void ChatMessage::save(const QUuid &chatId)
{
    QString query;
    if (m_isNew) {
        query = "INSERT INTO chat_messages (id, chat_id, message, messageDate, author) VALUES (:id, :chatId, :message, :messageDate, :author)";
    } else {
        query = "UPDATE chat_messages SET chat_id = :chatId, message = :message, messageDate = :messageDate, author = :author WHERE id = :id";
    }

    QSqlQuery sql(database);
    sql.prepare(query);
    sql.bindValue(":id", uuidToString(m_id));
    sql.bindValue(":chatId", uuidToString(chatId));
    sql.bindValue(":message", m_message);
    sql.bindValue(":messageDate", m_dateTime.toString(Qt::DateFormat::ISODate));
    sql.bindValue(":author", m_author);

    if (!sql.exec()) {
        logger->warning("Failed to save chat message, query: " + query + ", error: " + sql.lastError().text());
        qDebug() << "Failed to save chat message, query: " + query + ", error: " + sql.lastError().text();
        return;
    }

    m_isNew = false;
}
