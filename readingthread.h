#ifndef THREAD_H
#define THREAD_H

#include <QMutex>
#include <QThread>
#include <QFile>
#include <QMap>
#include "infostruct.h"

// типы кодировок файлов
enum class eCodec
{
    UTF8, Local8bit, Hex
};

class ReadingThread : public QThread
{
    Q_OBJECT

public:
    ReadingThread(QObject *parent = nullptr);
    ~ReadingThread();

    bool readFile(const QString &fileName);

signals:
    // сигнал в основной тред с информацией для гистограммы
    void outInfo(const QVector<Info> &info);

    void setCurrentProgress(double progress);

public slots:
    void stopProcess();

protected:
    void run();

private slots:
    void getNextInfo();

private:
    void getWords(const QByteArray &buffer, QByteArray &previous);

    void addWords(const QStringList &words);

    QFile file;
    // словарь со статистикой слов в файле
    QMap<QString, int> statistics;
    // При использовании QHash заполнение происходит быстрее,
    // но при обработке тратится время на сортировку слов
//    QHash <QString, int> statistics;

    // размер буфера чтения
    qint64 bufferSize;
    // максимальная допустимая длина слова
    qint64 maxWordLength;
    // количество строк, выводимых в гистограмме
    int maxLines;
    // символ разделителя слов
    char separator;
    // текущая кодировка файла
    eCodec currentCodec;

    // флаг остановки треда
    bool abort;
    QMutex mutex;
};

#endif // THREAD_H
