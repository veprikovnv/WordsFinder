import QtQuick 2.12
import QtQuick.Window 2.12

import "constants.js" as Constants

// элемент гистограммы
ListView {
    id: histogram

    anchors { fill: parent; margins: Constants.hist_bord }
    height: parent.height
    spacing: Constants.hist_bord

    // модель, получаемая из основного менеджера
    model: infoList
    delegate: Rectangle {
        width: parent.width
        // высота строки зависит от количества строк в представлении
        height: (histogram.height+Constants.hist_bord)/histogram.count - Constants.hist_bord

        // прямоугольник заливки шкалы гистограммы - ширина пропорциональна текущему количеству слов
        Rectangle {
            width: parent.width*modelData.proportion
            height: parent.height
            anchors.left: parent.left
            gradient: Gradient {
                orientation : Gradient.Horizontal
                GradientStop { position: 0.00; color: Constants.left_color }
                GradientStop { position: 1.00; color: Constants.right_color }
            }
        }

        // текущее слово
        Text{
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                margins: Constants.margins
            }
            text: modelData.word
        }

        // количество слов
        Text{
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                margins: Constants.margins
            }
            font.bold: true
            // если слов нет, то пустая строка
            text: (modelData.number > 0) ? modelData.number : ""
        }
    }
}
