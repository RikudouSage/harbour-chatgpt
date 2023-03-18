#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <QObject>
#include <QDateTime>
#include <QUuid>
#include <QSqlDatabase>

class ChatMessage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime dateTime MEMBER m_dateTime CONSTANT)
    Q_PROPERTY(Author author MEMBER m_author CONSTANT)
    Q_PROPERTY(QString message MEMBER m_message CONSTANT)
public:
    enum Author {
        User,
        ChatGPT,
    };
    Q_ENUM(Author)

    explicit ChatMessage(QDateTime dateTime, Author author, QString message, QObject *parent = nullptr);
    explicit ChatMessage(QUuid id, QDateTime dateTime, Author author, QString message, QObject *parent = nullptr);

    Author author();
    QString message();

    void save(const QUuid &chatId);

private:
    const QDateTime m_dateTime;
    const Author m_author;
    const QString m_message;
    const QUuid m_id;

    bool m_isNew = false;
    QSqlDatabase database = QSqlDatabase::database();
};

#endif // CHATMESSAGE_H
