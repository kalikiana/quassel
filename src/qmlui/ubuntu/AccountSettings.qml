import QtQuick 2.4
import Ubuntu.Components 1.3
import Quassel 0.1

Page {
    property AccountModel accountModel
    property int accountId

    id: settingsPage
    header: PageHeader {
        id: settingsHeader
        title: qsTr('Settings')
        clip: true
        leadingActionBar.actions: [
            Action {
                iconName: 'close'
                onTriggered: adaptivePageLayout.removePages(settingsPage)
            }
        ]
        trailingActionBar.actions: [
            Action {
                iconName: 'ok'
                text: qsTr('Save changes')
                enabled: hostnameInput.value && userInput.value && passwordInput.value
                onTriggered: {
                    accountModel.createOrUpdateAccount({
                        AccountId: accountId,
                        AccountName: accountNameInput.value,
                        HostName: hostnameInput.value,
                        Port: portInput.value,
                        User: userInput.value,
                        Password: passwordInput.value,
                    })
                    adaptivePageLayout.removePages(settingsPage)
                }
            }
        ]
    }

    ScrollView {
        width: parent.width
        anchors {
            top: settingsHeader.bottom
            bottom: parent.bottom
        }
        Flickable {
            anchors.fill: parent
            // add small buffer so last item isn't against the OSK/bottomedge
            contentHeight: settingsColumn.height + units.gu(4)
            Column {
                id: settingsColumn
                spacing: units.gu(2)
                anchors {
                    fill: parent
                    topMargin: units.gu(2)
                }
                clip: true
                ValueInput {
                    id: accountNameInput
                    label: qsTr('Account Name:')
                    text: accountModel.get(accountId).accountName
                    defaultValue: hostnameInput.value
                }
                ValueInput {
                    id: hostnameInput
                    label: qsTr('Hostname:')
                    text: accountModel.get(accountId).hostname
                }
                ValueInput {
                    id: portInput
                    label: qsTr('Port:')
                    text: accountModel.get(accountId).port
                    defaultValue: 4242
                }
                ValueInput {
                    id: userInput
                    label: qsTr('User:')
                    text: accountModel.get(accountId).user
                }
                ValueInput {
                    id: passwordInput
                    label: qsTr('Password:')
                    echoMode: TextInput.Password
                    text: accountModel.get(accountId).password
                }
                Button {
                    x: parent.spacing
                    width: parent.width - parent.spacing * 2
                    iconName: 'delete'
                    color: theme.palette.normal.negative
                    onTriggered: {
                        accountModel.removeAccount(accountId)
                        adaptivePageLayout.removePages(settingsPage)
                    }
                    visible: accountId
                }
            }
        }
    }
}
