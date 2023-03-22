import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Dialog {
    property alias chatTitle: chatTitleField.text

    id: page
    allowedOrientations: Orientation.All
    canAccept: chatTitle !== ''

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            DialogHeader {
                //: Dialog accept text
                acceptText: qsTr("Change title")
            }

            TextField {
                id: chatTitleField
                label: qsTr("New title")
            }
        }
    }

    Component.onCompleted: {
        logger.debug("Navigated to ChangeChatTitleDialog.qml");
    }
}
