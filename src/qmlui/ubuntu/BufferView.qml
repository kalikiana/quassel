import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    id: bufferPage
    property string themeNamespace: 'Ubuntu.Components.Themes.'
    header: PageHeader {
        title: qsTr('Quassel IRC')
        subtitle: statusbarMessage
        flickable: networksList
        clip: true
        trailingActionBar.actions: [
            Action {
                id: lightTheme
                iconName: 'torch-on'
                onTriggered: theme.name = themeNamespace + 'Ambiance'
                visible: theme.name.indexOf('SuruDark') > -1
            },
            Action {
                iconName: 'torch-off'
                onTriggered: theme.name = themeNamespace + 'SuruDark'
                visible: !lightTheme.visible
            }
        ]
    }

    UbuntuListView {
        id: networksList
        width: parent.width
        anchors {
            top: parent.top
            bottom: coreSelector.top
        }
        clip: true
        model: BufferModel { }
        highlight: null
        delegate: ListItem {
            id: networkItem
            height: networkLabel.height
            expansion.height: networkLabel.height + buffersList.height
            onClicked: expansion.expanded = !expansion.expanded
            Keys.forwardTo: [buffersList]
            Component.onCompleted: {
                networksList.ViewItems.expansionFlags = 0
                expansion.expanded = true
            }
            Column {
                anchors.fill: parent
                Label {
                    id: networkLabel
                    text: network
                    height: buffersList.labelHeight
                    width: parent.width
                }

                UbuntuListView {
                    id: buffersList
                    property real labelHeight: units.gu(4)
                    property real spacing: units.gu(2)
                    width: parent.width
                    height: count * (labelHeight + spacing)
                    visible: networkItem.expansion.expanded
                    model: buffers
                    delegate: ListItem {
                        id: bufferItem
                        height: bufferItemLayout.height
                        ListItemLayout {
                            id: bufferItemLayout
                            title.text: modelData.bufferName
                            ShortcutBubble {
                                action: bufferSwitchAction
                                SlotsLayout.position: SlotsLayout.Leading
                                visible: altPressed
                            }
                            ShortcutBubble {
                                visible: modelData.activity
                                color: UbuntuColors.orange
                                SlotsLayout.position: SlotsLayout.Trailing
                            }
                        }
                        action: Action {
                            id: bufferSwitchAction
                            onTriggered: {
                                buffersList.currentIndex = index
                                adaptivePageLayout.addPageToNextColumn(bufferPage, Qt.resolvedUrl('MessagesView.qml'), {
                                    bufferId: modelData.bufferId,
                                    networkId: modelData.networkId
                                })
                            }
                            shortcut: (modelData.bufferId < 10)
                                ? 'Alt+%1'.arg(modelData.bufferId)
                                : 'Alt+%1'.arg(String.fromCharCode(97 + modelData.bufferId - 10))
                        }
                    }
                }
            }
        }
    }

    // FIXME: Hide if there's only an internal core
    OptionSelector {
        id: coreSelector
        anchors {
            bottom: bufferPage.bottom
            topMargin: units.gu(0.5)
            bottomMargin: units.gu(0.5)
        }
        enabled: false // TODO
        width: parent.width
        model: AccountModel { }
        selectedIndex: model.lastAccountId
        delegate: OptionSelectorDelegate {
            text: accountName !== '' ? accountName : 'Core #%1'.arg(index)
            subText: '%1@%2'.arg(user).arg(hostname)
            iconName: useSecureConnection ? '' : '' // FIXME
        }
    }
}
