import QtQuick 2.11
import QtQuick.Window 2.11
import QtGraphicalEffects 1.0

import Home 1.0

Window {
    id: root
    visible: true
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    title: qsTr("Bluetooth Android/Rpi3")

    Bluetooth {
        id: bluetooth
    }

    Rectangle {
        id: visibleSpace
        height: root.height
        width: root.width
        color: "black"
    }

    Rectangle {
        color: "white"
        height: 1
        width: 0.8*root.width
        anchors.top: titleText.top
        anchors.topMargin: -0.02*root.height
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: titleText
        width: 0.8*root.width
        text: qsTr("Test bluetooth with rpi3")
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: root.height*0.1
        font.pixelSize: 24
        font.letterSpacing: 7
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        color: "white"
        height: 1
        width: 0.8*root.width
        anchors.bottom: titleText.bottom
        anchors.bottomMargin: -0.02*root.height
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: sensorText
        width: 0.8*root.width
        text: qsTr("Sensor value: ") + bluetooth.data
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 20
        font.letterSpacing: 2
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignLeft
    }



}
