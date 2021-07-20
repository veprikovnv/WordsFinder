#ifndef INFOSTRUCT_H
#define INFOSTRUCT_H

#include <QGuiApplication>

// структура для хранения информации об одной строке гистограммы
struct Info
{
    Q_GADGET
    // слово
    Q_PROPERTY(QString word MEMBER word)
    // текущее количество слов в файле
    Q_PROPERTY(int number MEMBER number)
    // пропорция текущего количества от максимума слов (от 0.0 до 1.0)
    Q_PROPERTY(double proportion MEMBER proportion)
public:
    QString word;
    int number;
    double proportion;
    Info(const QString& word="",
         int number = 0,
         double proportion = 0.0)
    {
        this->word = word;
        this->number = number;
        this->proportion = proportion;
    }
};
Q_DECLARE_METATYPE(Info)

#endif // INFOSTRUCT_H
