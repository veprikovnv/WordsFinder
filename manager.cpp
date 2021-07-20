#include "manager.h"

#include "infostruct.h"

#include <QDebug>

Manager::Manager(QObject *parent) :
    QObject(parent),
    maxLines(settings.value("max_output_words", 15).toInt()),
    thread(new ReadingThread(this))
{
    connect(this, &Manager::stop,
            thread, &ReadingThread::stopProcess);
    connect(thread, &ReadingThread::finished,
            this, &Manager::cancel);
    connect(thread, &ReadingThread::outInfo,
            this, &Manager::outInfo);
    connect(thread, &ReadingThread::setCurrentProgress,
            this, &Manager::setCurrentProgress);
}

void Manager::setContext(QQmlContext *cont)
{
    context = cont;

    // заполняем информацию о гистограмме
    for (int i = 0; i < maxLines; ++i)
        infoList << QVariant::fromValue(Info{});

    updateHistogram();
}

void Manager::openFile(const QString &fileName)
{
    qDebug() << "Manager::openFile" << fileName;

    if (thread->readFile(fileName))
    {
        emit switchButtons(true);

        emit setCurrentProgress(0.0);

        // сбрасываем информацию о гистограмме
        for (int i = 0; i < infoList.size(); ++i)
            infoList[i] = QVariant::fromValue(Info{});

        updateHistogram();
    }
}

void Manager::updateSettings()
{
    qDebug() << "Manager::updateSettings";

    // обновляем количество строк, выводимых в гистограмме
    maxLines = settings.value("max_output_words", 15).toInt();

    // сбрасываем информацию о гистограмме
    infoList.clear();
    for (int i = 0; i < maxLines; ++i)
        infoList << QVariant::fromValue(Info{});

    updateHistogram();

    emit setCurrentProgress(0.0);
}

void Manager::cancel()
{
    updateHistogram();

    emit switchButtons(false);
}

// слот вывода информации из треда чтения на гистограмму
void Manager::outInfo(const QVector<Info> &info)
{
    // обновляем информацию о гистограмме
    for (int i = 0; i < info.size(); ++i)
        if (i < infoList.size())
            infoList[i] = QVariant::fromValue(info[i]);

    updateHistogram();
}

void Manager::updateHistogram()
{
    context->setContextProperty("infoList", infoList);
}
