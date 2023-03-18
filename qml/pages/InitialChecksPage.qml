import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Page {
    property var doAfterLoad: []

    id: page
    allowedOrientations: Orientation.All

    function doAfterLoadOrNow(callable) {
        if (page.status === PageStatus.Active) {
            callable();
        } else {
            doAfterLoad.push(callable);
        }
    }

    SecretsHandler {
        id: secrets
    }

    ChatGptClient {
        id: chatGpt

        onApiKeyChecked: {
            if (isValid) {
                console.log(apiKey);
                secrets.setApiKey(apiKey);
                doAfterLoadOrNow(function() {
                    pageStack.replace("ChatListPage.qml");
                });
            } else {
                const login = pageStack.push("LoginPage.qml", {error: qsTr("The provided api key is invalid.")});
                login.accepted.connect(function() {
                    chatGpt.checkApiKey(login.apiKey);
                });
            }
        }
    }

    BusyLabel {
        id: loader
        running: true
        text: qsTr("Initializing...");
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                //: Page title
                title: qsTr("Initializing...")
            }
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Active) {
            while (doAfterLoad.length) {
                const callable = doAfterLoad.shift();
                callable();
            }
        }
    }

    Component.onCompleted: {
        doAfterLoadOrNow(function() {
            if (secrets.hasApiKey()) {
                pageStack.replace("ChatListPage.qml");
            } else {
                const login = pageStack.push("LoginPage.qml");
                login.accepted.connect(function() {
                    chatGpt.checkApiKey(login.apiKey);
                });
            }
        });
    }
}
