#include "chat.h"

#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

#include "uuidhelper.h"

Chat::Chat(const Chat &chat):
    QObject(chat.parent()),
    m_id(chat.m_id),
    m_createdDate(chat.m_createdDate),
    m_title(chat.m_title),
    m_database(QSqlDatabase::database()),
    isNew(false)
{
}

Chat::Chat(QObject *parent):
    QObject(parent),
    m_id(QUuid::createUuid()),
    m_createdDate(QDateTime::currentDateTime()),
    m_title(tr("Unnamed chat")),
    m_database(QSqlDatabase::database()),
    isNew(true)
{
}

Chat::Chat(QUuid id, QDateTime createdDate, QString title, QObject *parent):
    QObject(parent),
    m_id(id),
    m_createdDate(createdDate),
    m_title(title),
    m_database(QSqlDatabase::database()),
    isNew(false)
{
}

const QUuid Chat::id()
{
    return m_id;
}

QList<QObject*> Chat::messages()
{
    if (!messagesInitialized) {
        QSqlQuery query(m_database);
        query.prepare("SELECT * FROM chat_messages WHERE chat_id = :chatId ORDER BY messageDate ASC");
        query.bindValue(":chatId", uuidToString(m_id));
        query.exec();
        while (query.next()) {
            appendMessage(
                QUuid(query.value("id").toString()),
                query.value("message").toString(),
                static_cast<ChatMessage::Author>(query.value("author").toInt()),
                QDateTime::fromString(query.value("messageDate").toString(), Qt::DateFormat::ISODate)
            );
        }
        messagesInitialized = true;
    }
    return m_messages;
}


const QDateTime Chat::createdDate()
{
    return m_createdDate;
}

QString Chat::title()
{
    return m_title;
}

void Chat::setTitle(const QString &title)
{
    if (title != m_title) {
        m_title = title;
        emit titleChanged();
    }
}

void Chat::appendMessage(const QString &message, const int &author)
{
    appendMessage(message, static_cast<ChatMessage::Author>(author));
}

void Chat::appendMessage(const QString &message, const ChatMessage::Author &author, const QDateTime &dateTime)
{
    m_messages.append(new ChatMessage(dateTime, author, message, this));
    emit messagesChanged();
}

void Chat::appendMessage(const QUuid &id, const QString &message, const ChatMessage::Author &author, const QDateTime &dateTime)
{
    m_messages.append(new ChatMessage(id, dateTime, author, message, this));
}

void Chat::save()
{
    QString query;
    if (isNew) {
        query = "INSERT INTO chats (id, createdDate, title) VALUES (:id, :createdDate, :title)";
    } else {
        query = "UPDATE chats SET createdDate = :createdDate, title = :title WHERE id = :id";
    }

    QSqlQuery sql(m_database);
    sql.prepare(query);
    sql.bindValue(":id", uuidToString(m_id));
    sql.bindValue(":createdDate", m_createdDate.toString(Qt::DateFormat::ISODate));
    sql.bindValue(":title", m_title);

    if (!sql.exec()) {
        qDebug() << "Failed to exec query: " << query << sql.lastError().text();
    }

    for (const auto message : messages()) {
        static_cast<ChatMessage*>(message)->save(m_id);
    }

    isNew = false;
}
