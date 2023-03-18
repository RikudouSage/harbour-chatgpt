#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);

    static void createAndMigrate();
    static int getQuerySize(QSqlQuery &query);
private:
    static QSqlDatabase create();
    static void migrate(QSqlDatabase database);
    static int getLatestMigrationsVersion(const QSqlDatabase &database);
    static void applyMigration(const int &version, const QSqlDatabase &database);
};

#endif // DATABASEMANAGER_H
