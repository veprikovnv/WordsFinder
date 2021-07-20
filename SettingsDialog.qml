import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import Qt.labs.settings 1.0

import "constants.js" as Constants

// элемент настроек программы
Dialog {
    id: settingsDialog
    x: (window.width - width)*0.5
    y: (window.height - height)*0.5
    width: window.width*0.9
    modal: true
    focus: true
    title: qsTr("Settings")

    Settings {
        id: settings
        property int max_output_words: 15
        property int buffer_size: 16384
        property int max_word_length: 25
        property string text_codec: "UTF-8"
        property int words_separator: 32
        property bool show_settings: true
    }

    property int bufferSizeIndex: -1
    property int codecIndex: -1
    property int separatorIndex: -1

    standardButtons: Dialog.Ok | Dialog.Cancel
    // обновляем настройки программы
    onAccepted: {
        settings.max_output_words = wordsSlider.value
        settings.setValue("max_output_words", wordsSlider.value)

        settings.buffer_size = bufferSizeBox.currentValue
        settings.setValue("buffer_size", bufferSizeBox.currentValue)

        settings.max_word_length = wordLengthSlider.value
        settings.setValue("max_word_length", wordLengthSlider.value)

        settings.text_codec = codecBox.currentText
        settings.setValue("text_codec", codecBox.currentText)

        settings.words_separator = separatorBox.currentValue
        settings.setValue("words_separator", separatorBox.currentValue)

        settings.show_settings = showSettingsCheck.checked

        settingsDialog.close()
        manager.updateSettings()
    }
    // сбрасываем состояние элементов
    onRejected: {
        wordsSlider.value = settings.max_output_words
        bufferSizeBox.currentIndex = bufferSizeIndex
        wordLengthSlider.value = settings.max_word_length
        codecBox.currentIndex = codecIndex
        separatorBox.currentIndex = separatorIndex
        showSettingsCheck.checked = settings.show_settings

        settingsDialog.close()
    }

    // функция поиска ключа по значению
    function findKey(model, value) {
      for(var i = 0; i < model.count; ++i)
          if (model.get(i).value === value)
              return model.get(i).key
      return null
    }

    contentItem: ColumnLayout {
        id: settingsColumn
        spacing: Constants.settings_spacing*1.5

        RowLayout {
            spacing: Constants.settings_spacing

            Label {
                text: qsTr("Text codec:")
            }

            ComboBox {
                id: codecBox
                Layout.fillWidth: true
                model: ["UTF-8", "Local 8 bit", "Hex (binary files)"]
                Component.onCompleted: {
                    codecIndex = find(settings.text_codec)
                    if (codecIndex !== -1)
                        currentIndex = codecIndex
                }
            }
        }

        RowLayout {
            spacing: Constants.settings_spacing

            Label {
                text: qsTr("Words separator:")
            }

            ComboBox {
                id: separatorBox
                Layout.fillWidth: true
                textRole: "key"
                valueRole: "value"
                model: ListModel {
                    ListElement { key: qsTr("' ' (32) - space"); value: 32 }
                    ListElement { key: "'' (0)"; value: 0 }
                    ListElement { key: "'\\n' (10)"; value: 10 }
                    ListElement { key: "'0' (48)"; value: 48 }
                    ListElement { key: "'_' (95)"; value: 95 }
                }
                Component.onCompleted: {
                    var key = findKey(model, settings.words_separator)
                    separatorIndex = find(key)
                    if (separatorIndex !== -1)
                        currentIndex = separatorIndex
                }

            }
        }

        RowLayout {
            spacing: Constants.settings_spacing

            Label {
                text: qsTr("Max output words:")
            }

            Slider {
                id: wordsSlider
                from: 5
                value: settings.max_output_words
                to: 25
                stepSize: 1
                Layout.fillWidth: true
            }

            Text {
                text: wordsSlider.value
                Layout.minimumWidth: 20
            }

        }

        RowLayout {
            spacing: Constants.settings_spacing

            Label {
                text: qsTr("Max word length:")
            }

            Slider {
                id: wordLengthSlider
                from: 5
                value: settings.max_word_length
                to: 100
                stepSize: 1
                Layout.fillWidth: true
            }

            Text {
                text: wordLengthSlider.value
                Layout.minimumWidth: 20
            }

        }

        RowLayout {
            spacing: Constants.settings_spacing

            Label {
                text: qsTr("Reading buffer size:")
            }

            ComboBox {
                id: bufferSizeBox
                Layout.fillWidth: true
                model: [64, 256, 1024, 4096, 16384, 65536, 262144, 1048576]
                Component.onCompleted: {
                    bufferSizeIndex = find(settings.buffer_size)
                    if (bufferSizeIndex !== -1)
                        currentIndex = bufferSizeIndex
                }
            }
        }

        CheckBox {
            id: showSettingsCheck
            checked: settings.show_settings
            text: qsTr("Show settings at start")
        }
    }
}


