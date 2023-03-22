import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0
import cz.chrastecky.logs 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    property string error: ''
    property var logs: []
    property string exportBasename

    LogsExporter {
        id: exporter

        onErrorOpeningLogFile: {
            error = qsTr("There was an error when trying to open the export file '%1' in your Documents folder for writing.").arg(basename);
        }

        onExportFinished: {
            exportBasename = basename;
            exportSuccessMessage.visible = true;
        }
    }

    Connections {
        target: logger

        onErrorGettingLogs: {
            page.error = error;
            logs = [];
        }
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        PullDownMenu {
            MenuItem {
                text: qsTr("Delete logs");
                onClicked: {
                    logger.deleteLogs();
                    logs = logger.getLogs();
                }
            }

            MenuItem {
                text: qsTr("Export to file")
                onClicked: {
                    exporter.exportToFile(logger.getLogs());
                }
            }
        }

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                //: Page title
                title: qsTr("Logs")
            }

            Label {
                id: exportSuccessMessage
                x: Theme.horizontalPageMargin
                textFormat: Text.RichText;
                width: parent.width - Theme.horizontalPageMargin * 2
                text: "<style>a {color: " + Theme.highlightColor + "}</style>" + qsTr("Logs have been exported! You should find them in your Documents folder under the name '%1'.").arg(exportBasename) + "<br><br>" + qsTr("Click <a href='%2'>here</a> to contact the developer via email.").arg('mailto:dominik@chrastecky.cz');
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                visible: false
                onLinkActivated: {
                    Qt.openUrlExternally(link);
                }
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
                text: qsTr("There are no logs.");
                color: Theme.highlightColor
                wrapMode: Text.WordWrap
                visible: !logs.length
            }

            Repeater {
                model: logs

                delegate: Label {
                    property var item: logs[index]
                    function logLevelName(level) {
                        switch(level) {
                        case LogLevel.Debug:
                            return "DEBUG";
                        case LogLevel.Warning:
                            return "WARNING";
                        case LogLevel.Error:
                            return "ERROR";
                        }

                        return "UNKNOWN";
                    }

                    x: Theme.horizontalPageMargin
                    textFormat: Text.RichText
                    width: parent.width - Theme.horizontalPageMargin * 2
                    text: "<strong>[" + item.datetime + "] " + logLevelName(item.logLevel) + "</strong> " + item.message
                    wrapMode: Text.WordWrap
                }
            }
        }
    }

    Component.onCompleted: {
        logs = logger.getLogs();
    }
}
