import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: homePage 
    background: Rectangle { color: "black"}

    readonly property int titleFontSize: Math.round(height * .06)
    readonly property int baseFontSize: Math.round(height * 0.035)
    readonly property int buttonHeight: Math.round(height * 0.12)
    readonly property int pageMargin: Math.round(height * .03)

    ColumnLayout{
        anchors.fill: parent
        anchors.margins: homePage.pageMargin
        spacing: homePage.pageMargin

        Label{
            text: "Ground Control"
            color: "green"
            font.pixelSize: homePage.titleFontSize
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout{
            Layout.fillWidth: true
            spacing: homePage.pageMargin
            
            Label { text: "Link: No Link";  color: "steelBlue";     font.pixelSize: homePage.baseFontSize}
            Label { text: "Mode: N/A";      color: "red";           font.pixelSize: homePage.baseFontSize}
        }

        Button{
            text: "Display Stats"
            Layout.fillWidth: true
            Layout.preferredHeight: homePage.buttonHeight
            onClicked: homePage.StackView.view.push("StatsPage.qml")
        }

        Button {
            text: "Fault Injection"
            Layout.fillWidth: true
            Layout.preferredHeight: homePage.buttonHeight
        }

        Button {
            text: "Connection"
            Layout.fillWidth: true
            Layout.preferredHeight: homePage.buttonHeight
        }

        Item { Layout.fillHeight: true}
    }
}