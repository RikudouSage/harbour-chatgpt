#include "databasemanager.h"

#include <QDir>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

#include "logginghandler.h"

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent)
{

}

void DatabaseManager::createAndMigrate()
{
    auto db = create();
    migrate(db);

    LoggingHandler().debug("Migrations applied");
}

QSqlDatabase DatabaseManager::create()
{
    const LoggingHandler logger;
    QDir dbDir = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation);
    if (!dbDir.exists()) {
        dbDir.mkdir(".");
        logger.debug("Database dir does not exist, creating it");
    }
    auto mainDb = QSqlDatabase::addDatabase("QSQLITE");
    mainDb.setDatabaseName(dbDir.path() + "/maindb.sqlite");
    if (!mainDb.open()) {
        logger.error("There was an error when opening database:" + mainDb.lastError().text());
        qFatal("%s", mainDb.lastError().text().toStdString().c_str());
    }

    return mainDb;
}

void DatabaseManager::migrate(QSqlDatabase database)
{
    const LoggingHandler logger;

    QSqlQuery createMigrationsTableQuery("CREATE TABLE IF NOT EXISTS migrations (version INT)", database);
    if (!createMigrationsTableQuery.exec()) {
        logger.error("Failed to create migrations table");
        qFatal("Failed to create migrations table");
        return;
    }

    auto latestVersion = getLatestMigrationsVersion(database);

    if (latestVersion == 0) {
        QSqlQuery query(database);
        if (
                !query.exec("CREATE TABLE chats (id TEXT NOT NULL PRIMARY KEY, createdDate TEXT, title TEXT)")
                || !query.exec("CREATE TABLE chat_messages (id TEXT NOT NULL PRIMARY KEY, chat_id TEXT NOT NULL REFERENCES chats(id) ON DELETE CASCADE ON UPDATE CASCADE, message TEXT NOT NULL, messageDate TEXT NOT NULL, author INT NOT NULL)")
        ) {
            logger.error("Failed to migrate to version 1");
            qFatal("Failed to migrate to version 1");
        }

        applyMigration(1, database);
        latestVersion = getLatestMigrationsVersion(database);
    }

    if (latestVersion == 1) {
        QSqlQuery query(database);
        if (!query.exec("CREATE TABLE settings (setting TEXT NOT NULL PRIMARY KEY, value TEXT)")) {
            logger.error("Failed to migrate to version 2");
            qFatal("Failed to migrate to version 2");
        }

        applyMigration(2, database);
        latestVersion = getLatestMigrationsVersion(database);
    }

    if (latestVersion == 2) {
        QSqlQuery query(database);
        if (!query.exec("CREATE TABLE logs (message TEXT, datetime TEXT, level INT)")) {
            logger.error("Failed to migrate to version 3");
            qFatal("Failed to migrate to version 3");
        }

        applyMigration(3, database);
        latestVersion = getLatestMigrationsVersion(database);
    }
}

int DatabaseManager::getQuerySize(QSqlQuery &query)
{
    if (!query.isActive()) {
        return -1;
    }
    const auto originalPosition = query.at();
    query.last();
    const auto size = query.at() + 1;
    query.seek(originalPosition);

    return size;
}

int DatabaseManager::getLatestMigrationsVersion(const QSqlDatabase &database)
{
    int latestVersion;
    QSqlQuery getLatestMigrationQuery("SELECT MAX(version) as max FROM migrations", database);
    if (getQuerySize(getLatestMigrationQuery) == 0) {
        latestVersion = 0;
    } else if (getLatestMigrationQuery.next()) {
        latestVersion = getLatestMigrationQuery.value("max").toInt();
    } else {
        latestVersion = 0;
    }

    return latestVersion;
}

void DatabaseManager::applyMigration(const int &version, const QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.prepare("INSERT INTO migrations VALUES (:version)");
    query.bindValue(":version", version);
    query.exec();
}
