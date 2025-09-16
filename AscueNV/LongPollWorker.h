#pragma once

#include <QObject>
#include <QString>
#include <tgbot/tgbot.h>
#include <QThread>
#include <QTimer>

class LongPollWorker : public QObject
{
    Q_OBJECT
public:
    explicit LongPollWorker(QString any, QObject* parent = nullptr);
    ~LongPollWorker();

public slots:
    void doLongPoll();  // Запуск лонгпола, слот для потока
    void sendMessegeInTg(int64_t chatId, const std::string& message);
    void sendPhotoInTg(int64_t chatId, const std::string& message, const std::string& mime);
    void stopLongPoll();
    void timerPoll();

signals:
    void messageReceived(const TgBot::Message::Ptr& message);
    void errorOccurred(const QString& error);
signals:
    void finished();
signals:
    void resetWatchDogs();

private:
    TgBot::Bot* bot_;
    TgBot::TgLongPoll* longPoll;
    bool m_stopRequested = false;
    QTimer* pollTImer = nullptr;
};