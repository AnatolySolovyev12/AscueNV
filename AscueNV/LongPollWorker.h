#pragma once

#include <QObject>
#include <QString>
//#include <tgbot/tgbot.h>
#include <QThread>
#include <QtConcurrent>
#include <MaxClass.h>
#include <MessegeStruct.h>


class LongPollWorker : public QObject
{
    Q_OBJECT
public:
    explicit LongPollWorker(QString any, QObject* parent = nullptr);
    ~LongPollWorker();

public slots://////////////
    void doLongPoll();  // Запуск лонгпола, слот для потока
    void stopLongPoll();

public slots:
    void sendMessegeInTg(int64_t chatId, const std::string& message);
    void sendPhotoInTg(int64_t chatId, const std::string& message, const std::string& mime);


signals:////////////////
    void errorOccurred(const QString& error);
    void finished();
    void resetWatchDogs();

signals:
   // void messageReceived(const TgBot::Message::Ptr& message);
    void messageReceived(QSharedPointer<MyMessageObj>);

    void sendMessegeSignal(QString chatId, const QString& message);
    void sendImageSignal(const QString& chatId, const QString& message, const QString& mime);

private:
   // TgBot::Bot* bot_ = nullptr;
   // TgBot::TgLongPoll* longPoll = nullptr;
    bool m_stopRequested = false;
    MaxClass* maxClass = nullptr;
  //  TgBot::Message::Ptr message = std::make_shared<TgBot::Message>();
  //  std::shared_ptr<TgBot::Chat> chat = std::make_shared<TgBot::Chat>();
};