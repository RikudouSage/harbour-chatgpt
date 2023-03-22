#ifndef CHATSTORAGE_H
#define CHATSTORAGE_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>

#include "chat.h"
#include "logginghandler.h"
#include "settings.h"

class ChatStorage : public QObject
{
    Q_OBJECT
public:
    explicit ChatStorage(QObject *parent = nullptr);

    Q_INVOKABLE QList<QObject*> getChats();
    Q_INVOKABLE QObject* newChat();
    Q_INVOKABLE void removeChat(const QUuid &id);

private:
    QSqlDatabase database;
    const LoggingHandler *logger = new LoggingHandler(this);
    Settings *settings = new Settings(this);
};

#endif // CHATSTORAGE_H
