import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    property var chats: []

    ChatStorage {
        id: chatStorage
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                visible: settings.enableLogging
                text: qsTr("Logs")
            }

            MenuItem {
                text: qsTr("Settings")
                onClicked: {
                    pageStack.push("SettingsPage.qml");
                }
            }

            MenuItem {
                text: qsTr("New chat")
                onClicked: {
                    pageStack.push("ChatDetailPage.qml");
                }
            }
        }

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                //: Page title
                title: qsTr("Chat list")
            }

            Repeater {
                model: chats

                delegate: ListItem {
                    readonly property var item: chats[index];

                    id: listItem
                    contentHeight: Theme.itemSizeMedium
                    menu: contextMenu

                    onClicked: {
                        pageStack.push("ChatDetailPage.qml", {chat: item});
                    }

                    function remove() {
                        logger.debug("Deleting chat with id " + item.id);
                        remorseDelete(function() {
                            chatStorage.removeChat(item.id);
                            chats = chatStorage.getChats();
                            logger.debug("Chat with id " + item.id + " deleted");
                        });
                    }

                    Label {
                        id: itemTitle
                        text: item.title
                        x: Theme.horizontalPageMargin
                        width: page.width - Theme.horizontalPageMargin * 2
                        truncationMode: TruncationMode.Fade
                        y: Theme.paddingSmall
                    }

                    Label {
                        anchors.top: itemTitle.bottom
                        text: item.createdDate.toLocaleString(Qt.locale(), Locale.ShortFormat)
                        x: Theme.horizontalPageMargin
                        font.pixelSize: Theme.fontSizeSmall
                        color: Theme.secondaryHighlightColor
                    }

                    Image {
                        id: image
                        visible: parent.enabled
                        anchors {
                            right: parent.right
                            verticalCenter: parent.verticalCenter
                            rightMargin: Theme.horizontalPageMargin
                        }
                        source: "image://theme/icon-m-right?" + Theme.primaryColor
                    }

                    Component {
                         id: contextMenu
                         ContextMenu {
                             IconMenuItem {
                                 text: qsTr("Remove")
                                 icon.source: "image://theme/icon-m-remove"

                                 onClicked: {
                                     listItem.remove();
                                 }
                             }
                         }
                     }
                }
            }

            Label {
                text: qsTr("There are no chats, use pull down menu to create one.")
                x: Theme.horizontalPageMargin
                width: page.width - Theme.horizontalPageMargin * 2
                visible: chats.length === 0
                wrapMode: Text.WordWrap
            }
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Active) {
            chats = chatStorage.getChats();
        }
    }

    Component.onCompleted: {
        chats = chatStorage.getChats();
        logger.debug("Navigated to ChatListPage.qml");
    }
}
