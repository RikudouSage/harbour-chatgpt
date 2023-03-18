import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    SecretsHandler {
        id: secrets
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
                title: qsTr("Api key help")
            }

            Label {
                x: Theme.horizontalPageMargin
                textFormat: Text.RichText;
                width: parent.width - Theme.horizontalPageMargin * 2
                text: "<style>a {color: " + Theme.highlightColor + "}</style>" + qsTr("To get an api key, first go to <a href='%1'>%1</a> and create an account or log in to your existing one.").arg("https://platform.openai.com/account/api-keys")
                color: Theme.primaryColor
                wrapMode: Text.WordWrap
                onLinkActivated: {
                    Qt.openUrlExternally(link);
                }
            }

            Label {
                x: Theme.horizontalPageMargin
                textFormat: Text.RichText;
                width: parent.width - Theme.horizontalPageMargin * 2
                text: qsTr("Afterwards click the <strong>Create new secret key</strong> button and copy it from the resulting text box. The key won't be shown again so make sure you have copied it.")
                color: Theme.primaryColor
                wrapMode: Text.WordWrap
            }

            Label {
                x: Theme.horizontalPageMargin
                textFormat: Text.RichText;
                width: parent.width - Theme.horizontalPageMargin * 2
                text: qsTr("You can now go to the previous page in this application and paste the api key into the api key field.")
                color: Theme.primaryColor
                wrapMode: Text.WordWrap
            }
        }
    }
}
