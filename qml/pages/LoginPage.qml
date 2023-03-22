import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Dialog {
    property string apiKey: ''
    property string error

    id: page
    allowedOrientations: Orientation.All
    canAccept: apiKey !== ''

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            DialogHeader {
                //: Dialog accept text
                acceptText: qsTr("Save api key")
            }

            Label {
                x: Theme.horizontalPageMargin
                textFormat: Text.RichText;
                width: parent.width - Theme.horizontalPageMargin * 2
                text: error
                color: Theme.errorColor
                wrapMode: Text.WordWrap
                visible: error
            }

            Label {
                x: Theme.horizontalPageMargin
                textFormat: Text.RichText;
                width: parent.width - Theme.horizontalPageMargin * 2
                text: qsTr("To use this app you need to provide your own ChatGPT api key. <a href='#help' style='color: %1;'>Click here</a> to get help with getting the api key.").arg(Theme.highlightColor)
                color: Theme.primaryColor
                wrapMode: Text.WordWrap
                onLinkActivated: {
                    if (link === "#help") {
                        pageStack.push("ApiKeyHelpPage.qml");
                    }
                }
            }

            TextField {
                property bool passwordVisible: false

                id: apiKeyField
                label: qsTr("Api key")
                echoMode: passwordVisible ? TextInput.Normal : TextInput.Password
                rightItem: IconButton {
                    icon.source: !apiKeyField.passwordVisible
                                 ? "image://theme/icon-splus-hide-password"
                                 : "image://theme/icon-splus-show-password"
                    onClicked: {
                        apiKeyField.passwordVisible = !apiKeyField.passwordVisible;
                    }
                }

                onTextChanged: {
                    apiKey = text;
                }

                EnterKey.iconSource: "image://theme/icon-m-enter-accept"
                EnterKey.onClicked: {
                    page.accept();
                }
            }
        }
    }

    onRejected: {
        Qt.quit();
    }

    Component.onCompleted: {
        logger.debug("Navigated to LoginPage.qml");
    }
}
