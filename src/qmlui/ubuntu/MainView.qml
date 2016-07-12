import QtQuick 2.4
import Ubuntu.Components 1.3

MainView {
    width: units.gu(100)
    height: units.gu(75)
    anchorToKeyboard: true

    Binding {
        target: typeof window !== 'undefined' ? window : null
        property: 'title'
        value: qsTr('Quassel IRC')
    }

    focus: true
    property bool altPressed
    property bool controlPressed
    property bool shiftPressed
    Keys.onPressed: {
        altPressed = (event.key == Qt.Key_Alt || event.key == Qt.Key_AltGr)
        controlPressed = (event.key == Qt.Key_Control)
        shiftPressed = (event.key == Qt.Key_Shift)
    }
    Keys.onReleased: altPressed = controlPressed = shiftPressed = false
    onActiveFocusChanged: altPressed = controlPressed = shiftPressed = false

    AdaptivePageLayout {
        id: adaptivePageLayout
        anchors.fill: parent
        primaryPageSource: Qt.resolvedUrl('BufferView.qml')
        asynchronous: false
    }
}
