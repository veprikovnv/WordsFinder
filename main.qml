import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import Qt.labs.settings 1.0
import Qt.labs.platform 1.0

import "constants.js" as Constants

// основное окно приложения
Window {
    id: window
    visible: true
    width: 480
    height: 480
    title: qsTr("Words Finder")

    Connections {
        target: manager
        // обновляем информацию о прогрессе чтения файла
        onSetCurrentProgress: {
            progressText.text = (progress*100).toFixed(2) + "%"
            progressBar.value = progress
        }
        // переключаем кнопки:
        // - когда идёт чтение, кнопки "Открыть" и "Настройки" не доступны
        // - когда чтение остановлено, кнопка "Отмены" не доступна
        onSwitchButtons: {
            buttonOpen.enabled = !state
            buttonCancel.enabled = state
            buttonSettings.enabled = !state
        }
    }

    Settings {
        id: settings
        property bool show_settings: true
    }

    // полоска с кнопками и прогрессбаром
    Rectangle {
        id: toolbar

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: Constants.margins
        }

        height: 30

        Button {
            id: buttonOpen
            height: parent.height
            anchors.left: parent.left
            width: parent.width*0.2
            text: qsTr("Open...")
            onClicked: {
                fileDialog.open()
            }

            enabled: true
        }

        Button {
            id: buttonCancel
            height: parent.height
            anchors {
                right: buttonSettings.left
                margins: Constants.margins
            }
            width: parent.width*0.2
            text: qsTr("Cancel")
            onClicked: manager.stop()

            enabled: false
        }

        Button {
            id: buttonSettings
            height: parent.height
            anchors.right: parent.right
            width: parent.width*0.08
            Image {
//                source: "qrc:/settings.png"
                source: (buttonSettings.enabled) ? "qrc:/settings.png" : "qrc:/settings_off.png"
                anchors.fill: parent
                fillMode: Image.PreserveAspectFit
                anchors.margins: 3
            }
            onClicked: {
                settingsDialog.open()
            }
        }

        ProgressBar {
            id: progressBar
            height: parent.height
            anchors {
                left: buttonOpen.right
                right: buttonCancel.left
                margins: Constants.margins
            }
            value: 0.0

            Text {
                id: progressText
                anchors.centerIn: parent
//                text: Number(123.4398) + "%"
                text: "0.00%"
                z: 1
            }
        }
     }

    // контейнер для элемента гистограммы
    Rectangle {
        id: main_area
        color: Constants.bord_color

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            margins: Constants.margins
        }

        height: window.height - toolbar.height - Constants.margins*3

        Histogram {}
    }

    FileDialog {
        visible: false
        id: fileDialog
        title: qsTr("Open any file...")
        fileMode: FileDialog.OpenFile
        // путь к стандартной папке с документами
        //folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            // удаляем лишние символы из адреса
            var path = fileDialog.file.toString();
            path = path.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,"");
            path = decodeURIComponent(path);

            window.title = qsTr("Words Finder") + " - " + path;

            manager.openFile(path)
        }
    }

    SettingsDialog {
        id: settingsDialog
    }

    // показываем окно настроек, если установлен такой параметр
    Component.onCompleted: {
        if (settings.show_settings)
            settingsDialog.open()
    }

}
