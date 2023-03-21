import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.Layouts 1.1

import cz.chrastecky.chatgpt 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    property var chat: null
    property string messageInProgress: ''

    ChatStorage {
        id: chatStorage
    }

    ChatGptClient {
        id: chatGpt

        onMessageSent: {
            chatPrompt.enabled = false;
        }

        onMessageFinished: {
            chatPrompt.enabled = true;
            chat.appendMessage(messageInProgress, ChatMessage.ChatGPT, new Date());
            messageInProgress = '';
            chat.save();
            flickable.scrollToBottom();
        }

        onChunkReceived: {
            messageInProgress += chunk;
            flickable.scrollToBottom();
        }
    }

    SilicaFlickable {
        id: flickable

        contentHeight: column.height
        width: parent.width
        height: parent.height - chatPrompt.height - Theme.paddingMedium

        PullDownMenu {
            MenuItem {
                text: qsTr("Change title")
                onClicked: {
                    const dialog = pageStack.push("ChangeChatTitleDialog.qml", {chatTitle: chat.title});
                    dialog.accepted.connect(function() {
                        chat.title = dialog.chatTitle;
                        chat.save();
                    });
                }
            }
        }

        ColumnLayout {
            id: column

            width: page.width - Theme.horizontalPageMargin * 2
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: Theme.paddingLarge
            PageHeader {
                id: pageHeader
                //: Page title
                title: chat ? chat.title : qsTr("Chat")
            }

            Repeater {
                id: repeater
                property var messages: chat.messages
                model: messages

                delegate: ListItem {
                    property var item: repeater.messages[index]
                    width: column.width * 0.7
                    Layout.alignment: item.author === ChatMessage.User ? Qt.AlignRight : Qt.AlignLeft
                    contentHeight: message.height + Theme.paddingSmall * 2

                    Rectangle {
                        color: Theme.highlightBackgroundColor
                        opacity: Theme.highlightBackgroundOpacity
                        anchors.fill: parent
                    }

                    Label {
                        id: message
                        text: item.message
                        width: parent.width - Theme.paddingSmall * 2
                        wrapMode: Text.WordWrap
                        x: Theme.paddingSmall
                        y: Theme.paddingSmall
                        color: item.author === ChatMessage.User ? Theme.highlightColor : Theme.primaryColor
                    }
                }
            }

            ListItem {
                width: column.width * 0.7
                Layout.alignment: Qt.AlignLeft
                contentHeight: messageInProgressField.height + Theme.paddingSmall * 2
                visible: messageInProgress !== ''

                Rectangle {
                    color: Theme.highlightBackgroundColor
                    opacity: Theme.highlightBackgroundOpacity
                    anchors.fill: parent
                }

                Label {
                    id: messageInProgressField
                    text: messageInProgress
                    width: parent.width - Theme.paddingSmall * 2
                    wrapMode: Text.WordWrap
                    x: Theme.paddingSmall
                    y: Theme.paddingSmall
                    color: Theme.primaryColor
                }
            }
        }
    }

    TextField {
        id: chatPrompt
        width: parent.width - Theme.horizontalPageMargin * 2

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: Theme.horizontalPageMargin
        anchors.rightMargin: Theme.horizontalPageMargin

        label: qsTr("Type your message")

        EnterKey.iconSource: "image://theme/icon-m-enter-accept"
        EnterKey.onClicked: {
            focus = false;
            chatGpt.postMessage(chat, text);
            text = '';
        }
    }

    Component.onCompleted: {
        if (chat === null) {
            chat = chatStorage.newChat();
        }

        flickable.scrollToBottom();
    }
}
