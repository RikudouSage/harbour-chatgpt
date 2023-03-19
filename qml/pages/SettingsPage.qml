import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    Settings {
        id: settings
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
                title: qsTr("Settings")
            }

            TextSwitch {
                text: qsTr("Conversation length: %1").arg(settings.conversationLength)
                checked: true
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("Configure the length of the conversation that is sent to the ChatGPT model."),
                            qsTr("To maintain context, the AI model needs to see the previous messages as well, this configures how many past messages are sent."),
                            qsTr("Note that previous messages count towards the token limit of the AI model which is currently %1 tokens.").arg(4096),
                        ],
                        intFieldVisible: true,
                        intFieldDescription: qsTr("Conversation length (number of messages)"),
                        intValue: settings.conversationLength,
                    });

                    dialog.accepted.connect(function() {
                        const value = dialog.intValue;
                        settings.conversationLength = value;
                    });
                }
            }
        }
    }
}
