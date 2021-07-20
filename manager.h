#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QQmlContext>
#include <QSettings>
#include "readingthread.h"

// основной менеджер приложения
class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);

    void setContext(QQmlContext *cont);

signals:
    void stop();

    void setCurrentProgress(double progress);

    void switchButtons(bool state);

public slots:
    void openFile(const QString &fileName);

    void updateSettings();

    void cancel();

    // слот вывода информации из треда чтения на гистограмму
    void outInfo(const QVector<Info> &info);

private:
    void updateHistogram();

    QSettings settings;

    // количество строк, выводимых в гистограмме
    int maxLines;

    // хранилище информации о строках гистограммы
    QVariantList infoList;

    QQmlContext *context;

    ReadingThread *thread;
};

#endif // MANAGER_H
