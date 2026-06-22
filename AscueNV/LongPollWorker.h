#pragma once

#include <QObject>
#include <QString>
#include <QThread>
#include <QtConcurrent>
#include <MaxClass.h>
#include <MessegeStruct.h>


class LongPollWorker : public QObject
{
    Q_OBJECT
public:
    explicit LongPollWorker(QObject* parent = nullptr);

public slots:
    void doLongPoll();
    void stopLongPoll();
    void sendMessegeInTg(int64_t chatId, const std::string& message);
    void sendPhotoInTg(int64_t chatId, const std::string& message, const std::string& mime);

signals:
    void finished();
    void resetWatchDogs();
    void messageReceived(QSharedPointer<MyMessageObj>);
    void sendMessegeSignal(QString chatId, const QString& message);
    void sendImageSignal(const QString& chatId, const QString& message, const QString& mime);

private:
    bool m_stopRequested = false;
    MaxClass* maxClass = nullptr;
};