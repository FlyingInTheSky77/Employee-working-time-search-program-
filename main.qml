import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import Vis3Task.ControllerInQML 1.0

Window {
    width: 400
    height: 450
    minimumWidth: 400
    visible: true
    title: qsTr("Third task")

    readonly property int defpixelSize: 15

    Connections {
        target: my_Controller_in_QML
        function onSendToQMLSignal(message) {
            ti.append(addMsg(message));
       }
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 10
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            id: entersurname

            Rectangle{
                id: rec_surname
                Layout.fillWidth: true
                height: surnametext.height+20
                width: 50
            Text {
                    id: surnametext
                    text: qsTr("surname:")
                    font.pixelSize: defpixelSize
                    leftPadding: 5
                    rightPadding: 5
                    anchors.centerIn: rec_surname

                }
            }
            Rectangle {
                Layout.fillWidth: true
                height: surname.height+20
                radius: 5
                color: "#F4F2F5"
                border.color: "gray"
                border.width: 1
                width:100
                TextInput {
                    id: surname
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 10
                    rightPadding: 10
                    width: parent.width
                    font.pixelSize: defpixelSize
                    clip: true
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.leftMargin: 15
            Layout.rightMargin: 15
            id: enterdate

            Rectangle{
                id: rec_date
                Layout.fillWidth: true
                height: datetext.height+20
                width: 50

            Text {
                    id: datetext
                    text: qsTr("date:")
                    font.pixelSize: defpixelSize
                    leftPadding: 10
                    rightPadding: 10
                    anchors.centerIn: rec_date

                }
            }
            Rectangle {
                Layout.fillWidth: true
                height: date.height+20
                radius: 5
                color: "#F4F2F5"
                border.color: "gray"
                border.width: 1
                width:100
                TextInput {
                        id: date
                        property string placeholderText: "_2021-04-09"
                        anchors.verticalCenter: parent.verticalCenter
                        leftPadding: 10
                        rightPadding: 10
                        width: parent.width
                        font.pixelSize: defpixelSize
                        clip: true
                        Text{
                            text: date.placeholderText
                            color: "#aaa"
                            visible: !date.text
                        }
                }
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Layout.leftMargin: 15
            Layout.rightMargin: 15

            BetterButton {
                id:show_working_hours_button
                Layout.fillWidth: true
                Text {
                    text: qsTr("show working hours")
                    anchors.centerIn: parent
                }
                color: enabled ? "#86a4e5" : "#CED0D4"
                onClicked: {
                    ti.append(addMsg(my_Controller_in_QML.getWorkingMinutesByNameAndDate(surname.text,date.text)));
                    show_all_time_checkpoins_button.enabled = true;
                }
            }

            BetterButton {
                id:show_all_time_checkpoins_button
                Layout.fillWidth: true
                Text {
                    text: qsTr("show all time records")
                    anchors.centerIn: parent
                }
                color: enabled ? "#86a4e5" : "#CED0D4"
                onClicked: {
                    my_Controller_in_QML.showTimeCheckpoints();
                    this.enabled = false;
                }
            }
        }

        LayoutSection {
            Layout.fillHeight: true

            ScrollView {
                id: scrollView
                anchors.fill: parent

                TextArea {
                    id: ti
                    readOnly: true
                    selectByMouse : true
                    font.pixelSize: 14
                    wrapMode: TextInput.WrapAnywhere
                }
            }
        }
        BetterButton {
            id:exit_button
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Close program"
            color: this.down ? "#6FA3D2" : "#7DB7E9"
            border.color: "#6FA3D2"
            onClicked: {
                Qt.quit();
            }
        }
    }

    Component.onCompleted: {
        ti.text = addMsg("WorkingTime program started. Control system started from 2021-04");
        show_working_hours_button.enabled = true;
        show_all_time_checkpoins_button.enabled = false;
    }

    function addMsg(someText)
    {
        return "[" + currentTime() + "] " + someText;
    }

    function currentTime()
    {
        var now = new Date();
        var nowString = ("0" + now.getHours()).slice(-2) + ":"
                + ("0" + now.getMinutes()).slice(-2) + ":"
                + ("0" + now.getSeconds()).slice(-2);
        return nowString;
    }
}
