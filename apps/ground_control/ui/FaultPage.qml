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
    readonly property int pageMargin: Math.round(height * .03)
    readonly property int buttonHeight: Math.round(height * .1)
    readonly property int buttonFontSize: Math.round(height * .05)
    readonly property int faultCount: Math.max(1, faultRepeater.count)
    readonly property int headerFontSize: Math.round(height * .04)
    readonly property int rowHeight: Math.round((height - headerFontSize - titleFontSize - buttonHeight - pageMargin * (faultCount + 4)) / faultCount)
    readonly property int rowFontSize: Math.round(rowHeight * 0.4)
    readonly property int switchHeight: Math.round(Math.min(rowHeight * 0.5, width * 0.08))
    readonly property int valueWidth: Math.round(width * .18)

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

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            rows: faultPage.faultCount + 1
            flow: GridLayout.TopToBottom
            columnSpacing: faultPage.pageMargin * 2
            rowSpacing: faultPage.pageMargin

            // ───── LEFT COLUMN: header, then one row per sensor (label + value + red switch) ─────

            Label {
                text: "Inject (simulator)"
                color: "gray"
                font.pixelSize: faultPage.headerFontSize
                Layout.alignment: Qt.AlignRight
            }

            Repeater {
                id: faultRepeater
                model: groundControl.faultsModel

                delegate: RowLayout {
                    id: faultRow
                    Layout.fillWidth: true
                    Layout.preferredHeight: faultPage.rowHeight
                    spacing: faultPage.pageMargin * 2

                    required property int index
                    required property string label
                    required property string value
                    required property int status

                    Label {
                        text: faultRow.label
                        color: "white"
                        font.pixelSize: faultPage.rowFontSize
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 8
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                    }

                    Label {
                        text: faultRow.value
                        color: root.statusColor(faultRow.status)
                        font.pixelSize: faultPage.rowFontSize
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 8
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        Layout.preferredWidth: faultPage.valueWidth
                        Layout.fillHeight: true
                    }

                    Switch {
                        id: faultSwitch
                        Layout.preferredWidth: faultPage.switchHeight * 2
                        Layout.alignment: Qt.AlignVCenter
                        checked: faultRow.value === "On" || faultRow.value === "Turning On..."
                        onToggled: {
                            groundControl.SetFault(faultRow.index, checked);
                            checked = Qt.binding(function () {
                                return faultRow.value === "On" || faultRow.value === "Turning On...";
                            });
                        }

                        indicator: Rectangle {
                            implicitWidth: faultPage.switchHeight * 1.5
                            implicitHeight: faultPage.switchHeight * 0.75
                            anchors.verticalCenter: parent.verticalCenter
                            radius: height / 2
                            color: faultSwitch.checked ? "red" : "gray"

                            Rectangle {
                                width: parent.height
                                height: parent.height
                                radius: height / 2
                                color: "white"
                                x: faultSwitch.checked ? parent.width - width : 0
                            }
                        }
                    }
                }
            }

            // ───── RIGHT COLUMN: header, then one row per sensor (value + orange switch, no label) ─────

            Label {
                text: "Inhibit (flight computer)"
                color: "gray"
                font.pixelSize: faultPage.headerFontSize
                Layout.alignment: Qt.AlignRight
            }

            Repeater {
                id: inhibitRepeater
                model: groundControl.inhibitsModel

                delegate: RowLayout {
                    id: inhibitRow
                    Layout.preferredHeight: faultPage.rowHeight
                    spacing: faultPage.pageMargin * 2

                    required property int index
                    required property string value
                    required property int status

                    Label {
                        text: inhibitRow.value
                        color: root.statusColor(inhibitRow.status)
                        font.pixelSize: faultPage.rowFontSize
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 8
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                        Layout.preferredWidth: faultPage.valueWidth
                        Layout.fillHeight: true
                    }

                    Switch {
                        id: inhibitSwitch
                        Layout.preferredWidth: faultPage.switchHeight * 2
                        Layout.alignment: Qt.AlignVCenter
                        checked: inhibitRow.value === "On" || inhibitRow.value === "Turning On..."
                        onToggled: {
                            groundControl.SetInhibits(inhibitRow.index, checked);
                            checked = Qt.binding(function () {
                                return inhibitRow.value === "On" || inhibitRow.value === "Turning On...";
                            });
                        }

                        indicator: Rectangle {
                            implicitWidth: faultPage.switchHeight * 1.5
                            implicitHeight: faultPage.switchHeight * 0.75
                            anchors.verticalCenter: parent.verticalCenter
                            radius: height / 2
                            color: inhibitSwitch.checked ? "orange" : "gray"

                            Rectangle {
                                width: parent.height
                                height: parent.height
                                radius: height / 2
                                color: "white"
                                x: inhibitSwitch.checked ? parent.width - width : 0
                            }
                        }
                    }
                }
            }
        }

        Button {
            text: "Back"
            Layout.preferredWidth: faultPage.buttonHeight * 2
            Layout.preferredHeight: faultPage.buttonHeight
            font.pixelSize: faultPage.buttonFontSize
            onClicked: faultPage.StackView.view.pop()
        }
    }
}
