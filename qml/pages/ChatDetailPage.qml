import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.Layouts 1.1

import cz.chrastecky.chatgpt 1.0

import "../components" as Components

Page {
    id: page
    allowedOrientations: Orientation.All

    property var chat: null
    property string messageInProgress: ''
    property bool chatIndicatorVisible: false

    Components.Toaster {
        id: toaster
    }

    ChatStorage {
        id: chatStorage
    }

    ChatGptClient {
        id: chatGpt

        onMessageSent: {
            chatPrompt.enabled = false;
            chatIndicatorField.text = '.';
            chatIndicatorVisible = true;
        }

        onMessageFinished: {
            chatPrompt.enabled = true;
            chat.appendMessage(messageInProgress, ChatMessage.ChatGPT, new Date());
            messageInProgress = '';
            chat.save();
            flickable.scrollToBottom();
        }

        onChunkReceived: {
            chatIndicatorVisible = false;
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
                        logger.debug("Changed chat title to " + dialog.chatTitle);
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
                property var messages: chat.messages.filter(function(item) { return item.author !== ChatMessage.System })
                model: messages

                delegate: ListItem {
                    property var item: repeater.messages[index]

                    width: column.width * 0.7
                    Layout.alignment: item.author === ChatMessage.User ? Qt.AlignRight : Qt.AlignLeft
                    contentHeight: message.height + Theme.paddingSmall * 2

                    onClicked: {
                        Clipboard.text = message.text;
                        toaster.show(qsTr("The message has been copied to the clipboard."))
                    }

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

            ListItem {
                width: column.width * 0.7
                Layout.alignment: Qt.AlignLeft
                contentHeight: chatIndicatorField.height + Theme.paddingSmall * 2
                visible: chatIndicatorVisible

                Rectangle {
                    color: Theme.highlightBackgroundColor
                    opacity: Theme.highlightBackgroundOpacity
                    anchors.fill: parent
                }

                Label {
                    id: chatIndicatorField
                    width: parent.width - Theme.paddingSmall * 2
                    wrapMode: Text.WordWrap
                    x: Theme.paddingSmall
                    y: Theme.paddingSmall
                    text: ''
                    color: Theme.primaryColor
                }
            }
        }
    }

    Timer {
        readonly property int max: 5
        running: chatIndicatorVisible
        interval: 1000
        repeat: true

        onTriggered: {
            if (chatIndicatorField.text === '') {
                chatIndicatorField.text = '.';
                return;
            }
            if (chatIndicatorField.text.length === max) {
                chatIndicatorField.text = '.';
                return;
            }
            chatIndicatorField.text += '.';
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

        logger.debug("Navigated to ChatDetailPage.qml, chat id: " + chat.id);
    }
}
