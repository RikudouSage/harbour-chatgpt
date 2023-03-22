import QtQuick 2.0
import Sailfish.Silica 1.0

import "pages"
import "components"

ApplicationWindow {
    id: app
    initialPage: Component { InitialChecksPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}
