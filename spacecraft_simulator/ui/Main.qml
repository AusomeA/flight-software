import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import SpacecraftSimulator

ApplicationWindow {
    id: root

    /* Timer {
        interval: 100
        running: true
        repeat: false
        onTriggered: {
            root.x = (Screen.width - width) / 2;
            root.y = (Screen.height - height) / 2;
        }
    } */

    Spacecraft {
        id: simulator
        Component.onCompleted: Start()
    }

    function statusColor(status) {
        switch (status) {
        case Spacecraft.Status.good:
            return "green"; // good
        case Spacecraft.Status.warning:
            return "yellow"; // warning
        case Spacecraft.Status.critical:
            return "red"; // critical
        default:
            return "white"; // none / unknown
        }
    }

    visible: true
    visibility: Window.FullScreen

    title: "Spacecraft Simulator"

    color: "black"

    readonly property int rowHeight: Math.round(height * 0.1)
    readonly property int baseFontSize: Math.round(rowHeight * 0.45)
    readonly property int cellPadding: Math.round(baseFontSize * 0.5)

    // Esc Key exits full screen mode and returns to windowed mode, or vice versa
    Shortcut {
        sequence: "Esc"
        onActivated: root.visibility = root.visibility === Window.FullScreen ? Window.Windowed : Window.FullScreen
    }

    Shortcut {
        sequence: "Right"
        onActivated: simulator.IncreaseTimeScale()
    }

    Shortcut {
        sequence: "Left"
        onActivated: simulator.DecreaseTimeScale()
    }

    Shortcut {
        sequence: "Up"
        onActivated: simulator.ModeTestUp()
    }

    Shortcut {
        sequence: "Down"
        onActivated: simulator.ModeTestDown()
    }

    Shortcut {
        sequence: "="
        onActivated: simulator.BatteryTestUp()
    }

    Shortcut {
        sequence: "-"
        onActivated: simulator.BatteryTestDown()
    }

    Shortcut {
        sequence: "s,0"
        onActivated: simulator.ToggleSensorFault(0)
    }

    Shortcut {
        sequence: "s,1"
        onActivated: simulator.ToggleSensorFault(1)
    }

    Shortcut {
        sequence: "s,2"
        onActivated: simulator.ToggleSensorFault(2)
    }

    Shortcut {
        sequence: "f"
        onActivated: simulator.FailRandomSensor(Math.floor(Math.random() * 3))
    }

    Shortcut {
        sequence: "c"
        onActivated: simulator.ToggleChaosMode()
    }

    ScrollView {
        id: readoutsScrollView
        anchors.fill: parent
        anchors.margins: 20

        Column {
            id: readoutColumn
            width: readoutsScrollView.availableWidth
            spacing: 0

            Repeater {
                model: simulator.readoutsModel

                delegate: RowLayout {
                    id: readoutRow
                    width: readoutColumn.width
                    spacing: 0
                    required property string label
                    required property string value
                    required property int status

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: rowHeight
                        color: "black"
                        border.color: "white"
                        border.width: 1

                        Label {
                            anchors.fill: parent
                            anchors.margins: cellPadding
                            verticalAlignment: Label.AlignVCenter
                            horizontalAlignment: Label.AlignHCenter
                            elide: Text.ElideRight

                            text: readoutRow.label

                            font.pixelSize: baseFontSize
                            color: "white"
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: rowHeight
                        color: "black"
                        border.color: "white"
                        border.width: 1

                        Label {

                            anchors.fill: parent
                            anchors.margins: cellPadding
                            verticalAlignment: Label.AlignVCenter
                            horizontalAlignment: Label.AlignHCenter
                            elide: Text.ElideRight

                            text: readoutRow.value

                            font.pixelSize: baseFontSize
                            color: statusColor(readoutRow.status)
                        }
                    }
                }
            }
        }
    }
}
