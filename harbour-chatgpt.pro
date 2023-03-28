# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-chatgpt

CONFIG += sailfishapp
PKGCONFIG += sailfishsecrets sailfishcrypto
QT += sql

SOURCES += src/harbour-chatgpt.cpp \
    src/chat.cpp \
    src/chatgptclient.cpp \
    src/chatmessage.cpp \
    src/chatstorage.cpp \
    src/databasemanager.cpp \
    src/logginghandler.cpp \
    src/logsexporter.cpp \
    src/secretshandler.cpp \
    src/settings.cpp

DISTFILES += qml/harbour-chatgpt.qml \
    qml/cover/CoverPage.qml \
    qml/pages/ApiKeyHelpPage.qml \
    qml/pages/ChangeChatTitleDialog.qml \
    qml/pages/ChatDetailPage.qml \
    qml/pages/ChatListPage.qml \
    qml/pages/ConfirmSettingDialog.qml \
    qml/pages/InitialChecksPage.qml \
    qml/pages/LoginPage.qml \
    qml/pages/LogsPage.qml \
    qml/pages/SettingsPage.qml \
    rpm/harbour-chatgpt.changes.in \
    rpm/harbour-chatgpt.changes.run.in \
    rpm/harbour-chatgpt.spec \
    translations/*.ts \
    harbour-chatgpt.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-chatgpt-cs.ts \
                translations/harbour-chatgpt-sv.ts \
                translations/harbour-chatgpt-pl.ts

HEADERS += \
    src/chat.h \
    src/chatgptclient.h \
    src/chatmessage.h \
    src/chatstorage.h \
    src/databasemanager.h \
    src/logginghandler.h \
    src/logsexporter.h \
    src/secretshandler.h \
    src/settings.h \
    src/uuidhelper.h
