#include "secretshandler.h"

#include <Sailfish/Secrets/collectionnamesrequest.h>
#include <Sailfish/Secrets/createcollectionrequest.h>
#include <Sailfish/Secrets/result.h>
#include <Sailfish/Secrets/secret.h>
#include <Sailfish/Secrets/storesecretrequest.h>
#include <Sailfish/Secrets/storedsecretrequest.h>
#include <Sailfish/Secrets/deletesecretrequest.h>
#include <Sailfish/Secrets/deletecollectionrequest.h>

#include <QDebug>

using Sailfish::Secrets::CollectionNamesRequest;
using Sailfish::Secrets::SecretManager;
using Sailfish::Secrets::Request;
using Sailfish::Secrets::Result;
using Sailfish::Secrets::CreateCollectionRequest;
using Sailfish::Secrets::Secret;
using Sailfish::Secrets::StoreSecretRequest;
using Sailfish::Secrets::StoredSecretRequest;
using Sailfish::Secrets::DeleteSecretRequest;
using Sailfish::Secrets::DeleteCollectionRequest;

const QString SecretsHandler::collectionName(QStringLiteral("chatgpt"));

static const QString apiKeyName = "apiKey";

SecretsHandler::SecretsHandler(QObject *parent) : QObject(parent)
{
    CollectionNamesRequest cnr;
    cnr.setManager(secretManager);
    cnr.setStoragePluginName(SecretManager::DefaultEncryptedStoragePluginName);
    cnr.startRequest();
    cnr.waitForFinished();

    hasCollection = isResultValid(cnr) && cnr.collectionNames().contains(collectionName);
    logger->debug("Has secrets collection: " + QString(hasCollection ? "true" : "false"));
}

QString SecretsHandler::apiKey()
{
    return getData(apiKeyName);
}

bool SecretsHandler::hasApiKey()
{
    auto key = apiKey();
    return !key.isNull() && !key.isEmpty();
}

void SecretsHandler::setApiKey(const QString &apiKey)
{
    storeData(apiKeyName, apiKey);
}

void SecretsHandler::removeApiKey()
{
    deleteSecret(apiKeyName);
}

bool SecretsHandler::isResultValid(const Request &request)
{
    auto result = request.result();
    auto isSuccess = result.errorCode() == Result::NoError;
    if (!isSuccess) {
        logger->warning("Request for secret failed: " + result.errorMessage());
        qWarning() << "Request for secret failed: " + result.errorMessage();
    }

    return isSuccess;
}

bool SecretsHandler::isSecretValid(const Secret &secret)
{
    return !secret.name().isNull() && !secret.name().isEmpty();
}

bool SecretsHandler::storeData(const QString &name, const QString &data)
{
    if (!hasCollection) {
        createCollection();
        // todo handle case where collection isn't created
    }

    auto existingSecret = getSecret(name);
    if (isSecretValid(existingSecret)) {
        logger->debug("Secret " + name + " already exists, deleting it before creating it again.");
        deleteSecret(name);
    }

    Secret secret(toIdentifier(name));
    secret.setData(data.toUtf8());

    StoreSecretRequest ssr;
    ssr.setManager(secretManager);
    ssr.setSecretStorageType(StoreSecretRequest::CollectionSecret);
    ssr.setUserInteractionMode(SecretManager::SystemInteraction);
    ssr.setSecret(secret);
    ssr.startRequest();
    ssr.waitForFinished();

    const auto result = isResultValid(ssr);
    logger->debug("Creating secret " + name + "status: " + QString(result ? "true" : "false"));

    return result;
}

Secret SecretsHandler::getSecret(const QString &name)
{
    if (!hasCollection) {
        logger->debug("Secret with name " + name + " not found");
        return Secret();
    }

    StoredSecretRequest ssr;
    ssr.setManager(secretManager);
    ssr.setUserInteractionMode(SecretManager::SystemInteraction);
    ssr.setIdentifier(toIdentifier(name));
    ssr.startRequest();
    ssr.waitForFinished();

    auto success = isResultValid(ssr);
    if (!success) {
        logger->warning("Failed to get secret with name " + name);
        return Secret();
    }

    return ssr.secret();
}

bool SecretsHandler::deleteSecret(const QString &name)
{
    DeleteSecretRequest dsr;
    dsr.setManager(secretManager);
    dsr.setIdentifier(toIdentifier(name));
    dsr.setUserInteractionMode(SecretManager::SystemInteraction);
    dsr.startRequest();
    dsr.waitForFinished();

    const auto result = isResultValid(dsr);

    logger->debug("Deleting secret " + name + " result: " + QString(result ? "true" : "false"));

    return result;
}

QString SecretsHandler::getData(const QString &name)
{
    auto secret = getSecret(name);
    if (!isSecretValid(secret)) {
        return QString();
    }

    return QString::fromUtf8(secret.data());
}

bool SecretsHandler::createCollection()
{
    CreateCollectionRequest ccr;
    ccr.setManager(secretManager);
    ccr.setCollectionName(collectionName);
    ccr.setAccessControlMode(SecretManager::OwnerOnlyMode);
    ccr.setCollectionLockType(CreateCollectionRequest::DeviceLock);
    ccr.setDeviceLockUnlockSemantic(SecretManager::DeviceLockKeepUnlocked);
    ccr.setStoragePluginName(SecretManager::DefaultEncryptedStoragePluginName);
    ccr.setEncryptionPluginName(SecretManager::DefaultEncryptedStoragePluginName);
    ccr.startRequest();
    ccr.waitForFinished();

    auto success = isResultValid(ccr);
    hasCollection = success;

    logger->debug("Creating collection status: " + QString(success ? "true" : "false"));

    return success;
}

Secret::Identifier SecretsHandler::toIdentifier(const QString &name)
{
    return Secret::Identifier(name, collectionName, SecretManager::DefaultEncryptedStoragePluginName);
}
