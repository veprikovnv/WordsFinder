#include "readingthread.h"

#include <QDebug>
#include <QSettings>
#include <QDateTime>

ReadingThread::ReadingThread(QObject *parent) :
    QThread(parent),
    bufferSize(16384),
    maxWordLength(25),
    maxLines(15),
    separator(32),
    currentCodec(eCodec::UTF8)
{
    abort = false;
}

ReadingThread::~ReadingThread()
{
    // останавливаем тред
    mutex.lock();
    abort = true;
    mutex.unlock();

    wait();
}

bool ReadingThread::readFile(const QString &fileName)
{
    if (file.isOpen())
        file.close();

    emit setCurrentProgress(0.0);

    #ifdef Q_OS_LINUX
    file.setFileName("/" +fileName);
    #else
    file.setFileName(fileName);
    #endif

    if (!file.open(QIODevice::ReadOnly))
        return false;

    mutex.lock();
    abort = false;
    mutex.unlock();

    start();

    return true;
}

void ReadingThread::stopProcess()
{
    // останавливаем тред
    mutex.lock();
    abort = true;
    mutex.unlock();
}

void ReadingThread::run()
{
    // текущая позиция в файле
    qint64 position = 0;
    // буфер для накопления остатков от предыдущего чтения
    QByteArray previous;

    // сбрасываем статистику
    statistics.clear();

    // получаем настройки текущего процесса чтения файла
    QSettings settings;

    bufferSize = settings.value("buffer_size", 16384).toInt();
    maxWordLength = settings.value("max_word_length", 25).toInt();
    separator = settings.value("words_separator", 32).toInt();
    maxLines = settings.value("max_output_words", 15).toInt();

    QString text_codec = settings.value("text_codec", "UTF-8").toString();
    currentCodec = eCodec::UTF8;
    if (text_codec == "Local 8 bit")
        currentCodec = eCodec::Local8bit;
    else
    if (text_codec == "Hex (binary files)")
        currentCodec = eCodec::Hex;

    int counter = 0;

    bool current_abort = false;

    // рабочий цикл потока чтения файла
    while (position < file.size())
    {
        // считываем очередную порцию файла и получаем из неё слова
        QByteArray buffer = file.read(bufferSize);
        getWords(buffer, previous);

        mutex.lock();
        current_abort = abort;
        mutex.unlock();

        // если процесс остановлен
        if (current_abort)
            // выходим из цикла чтения
            break;

        // отдыхаем
        msleep(50);

        // обновляем процент обработки файла
        emit setCurrentProgress((double)position/file.size());

        position += bufferSize;

        ++counter;

        // раз в две итерации цикла обновляем информацию для гистограммы
        if (counter%2 == 0)
            getNextInfo();
    }

    file.close();

    // если было нормальное завершение чтения
    if (!current_abort)
    {
        // обрабатываем накопившийся остаток
        if (!previous.isEmpty() && previous.size() <= maxWordLength)
            addWords(QStringList(previous));

        getNextInfo();

        emit setCurrentProgress(1.0);
    }

}

void ReadingThread::getNextInfo()
{
    if (statistics.isEmpty())
        return;

    // информация для гистограммы
    QVector<Info> outVect;

    QStringList words = statistics.keys();
    QList <int> values = statistics.values();

    // сортируем количество накопленных слов
    std::sort(values.begin(), values.end());

    // отрезаем то, что больше количества срок гистограммы
    if (values.size() > maxLines)
        values = values.mid(values.size()-maxLines);

    // ключи в словаре QMap по умолчанию отсортированы по возрастанию
    // сортировка не требуется
//    words.sort();

    // максимальное количество слов
    int max = values.last();

    for (QString word : words)
    {
        int number = statistics[word];

        // если такого количества нет в списке для гистограммы
        if (!values.contains(number))
            // переходим к следующему
            continue;

        // удаляем данное количество
        values.removeOne(number);

        // добавляем информацию о данном слове
        outVect.append(Info{word, number, (double)number/max});
    }

    // передаём полученную информацию в основной менеджер
    emit outInfo(outVect);
}

void ReadingThread::getWords(const QByteArray &buffer, QByteArray &previous)
{
    QStringList words;

    // флаг очистки накопленного остатка
    bool prevCleared = false;

    if (previous.size() > maxWordLength)
    {
        previous.clear();
        prevCleared = true;
    }

    // добавляем в остаток новую информацию из буфера
    previous.append(buffer);

    // если есть разделитель слов
    if (previous.contains(separator))
    {
        // получаем список слов
        QList<QByteArray> list = previous.split(separator);

        // если остаток был очищен
        if (prevCleared)
            // то в первом слове хранится часть остатка, удаляем её
            list.removeFirst();

        // в остаток помещаем начало последнего слова
        previous = list.takeLast();

        for (QByteArray word : list)
        {
            if (word.size() > maxWordLength)
                continue;

            if (word.isEmpty())
                continue;

            // получаем из "сырой" информации слово в нужной кодировке
            QString str;

            switch (currentCodec)
            {
                case eCodec::UTF8:
                    str = QString::fromUtf8(word);
                    break;
                case eCodec::Local8bit:
                    str = QString::fromLocal8Bit(word);
                    break;
                default:
                    // для бинарного чтения переводим в шестнадцатеричное представление
                    str = word.toHex().toUpper();
            }

            words.append(str);
        }

        // добавляем прочитанные слова в словарь
        addWords(words);
    }
}

void ReadingThread::addWords(const QStringList &words)
{
    for (QString word: words)
    {
        if (statistics.contains(word))
            ++statistics[word];
        else
            statistics[word] = 1;
    }
}

