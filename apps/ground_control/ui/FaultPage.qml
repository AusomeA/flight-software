import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import GroundControl

Page {
    id: faultPage

    background: Rectangle {
        color: "black"
    }

    readonly property int titleFontSize: Math.round(height * .06)
    readonly property int baseFontSize: Math.round(height * 0.035)
    readonly property int rowHeight: Math.round(height * 0.12)
    readonly property int pageMargin: Math.round(height * .03)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: faultPage.pageMargin
        spacing: faultPage.pageMargin

        Label {
            text: "Fault Injection"
            color: "green"
            font.pixelSize: faultPage.titleFontSize
            Layout.alignment: Qt.AlignHCenter
        }

        Repeater {
            //id: readoutRepeater
            model: groundControl.faultsModel

            delegate: RowLayout {
                id: faultRow
                Layout.fillWidth: true
                Layout.preferredHeight: faultPage.rowHeight
                required property int index
                required property string label
                required property string value
                required property int status

                Label {
                    text: faultRow.label
                    color: "white"
                    font.pixelSize: faultPage.baseFontSize
                    Layout.fillWidth: true
                }

                Label {
                    text: faultRow.value
                    color: root.statusColor(faultRow.status)
                    font.pixelSize: faultPage.baseFontSize
                }

                Switch {
                    checked: faultRow.value === "On" || faultRow.value === "Turning On..."
                    onToggled: groundControl.SetFault(faultRow.index, checked)
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }

        Button {
            text: "Back"
            Layout.preferredWidth: 120
            Layout.preferredHeight: 60
            onClicked: faultPage.StackView.view.pop()
        }
    }
}
