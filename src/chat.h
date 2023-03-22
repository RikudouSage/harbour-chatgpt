#ifndef CHAT_H
#define CHAT_H

#include <QObject>
#include <QDateTime>
#include <QUuid>
#include <QSqlDatabase>
#include <QVariantList>

#include "chatmessage.h"
#include "logginghandler.h"

class Chat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid id MEMBER m_id CONSTANT)
    Q_PROPERTY(QDateTime createdDate MEMBER m_createdDate CONSTANT)
    Q_PROPERTY(QList<QObject*> messages READ messages NOTIFY messagesChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
public:
    Chat(const Chat &chat);
    explicit Chat(QObject *parent = nullptr);
    explicit Chat(QUuid id, QDateTime createdDate, QString title, QObject *parent = nullptr);

    const QUuid id();
    QList<QObject*> messages();
    const QDateTime createdDate();
    QString title();

    void setTitle(const QString &title);

    Q_INVOKABLE void appendMessage(const QString &message, const int &author);
    void appendMessage(const QString &message, const ChatMessage::Author &author, const QDateTime &dateTime = QDateTime::currentDateTime());
    void appendMessage(const QUuid &id, const QString &message, const ChatMessage::Author &author, const QDateTime &dateTime = QDateTime::currentDateTime());
    Q_INVOKABLE void save();

signals:
    void messagesChanged();
    void titleChanged();

private:
    const QUuid m_id;
    QList<QObject*> m_messages;
    const QDateTime m_createdDate;
    QString m_title;

    QSqlDatabase m_database;
    bool messagesInitialized = false;
    bool isNew;

    const LoggingHandler *logger = new LoggingHandler(this);
};

#endif // CHAT_H
