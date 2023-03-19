import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Dialog {
    property var messages: []

    property alias intFieldVisible: intField.visible
    property alias intFieldDescription: intField.label
    property alias intValue: intField.text

    canAccept: (intFieldVisible ? intValue.length > 0 : true)

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
            DialogHeader {
                //: Dialog accept text
                acceptText: qsTr("Confirm")
            }

            Repeater {
                model: messages
                delegate: Label {
                    property var item: messages[index]

                    x: Theme.horizontalPageMargin
                    textFormat: Text.RichText;
                    width: parent.width - Theme.horizontalPageMargin * 2
                    text: item
                    color: Theme.primaryColor
                    wrapMode: Text.WordWrap
                }
            }

            TextField {
                id: intField
                inputMethodHints: Qt.ImhDigitsOnly
            }
        }
    }
}
