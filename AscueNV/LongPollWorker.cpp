#include "LongPollWorker.h"
#include <QThread>
#include <qdebug.h>

LongPollWorker::LongPollWorker(TgBot::Bot* bot, QObject* parent)
    : QObject(parent), bot_(bot)
{
    longPoll_ = new TgBot::TgLongPoll(*bot_, 100, 8);
    
    bot_->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        emit messageReceived(message);
        });
        
    printf("Bot username: %s\n\n", bot_->getApi().getMe()->username.c_str());
}

LongPollWorker::~LongPollWorker()
{
    delete longPoll_;
}

void LongPollWorker::doLongPoll()
{
    try
    {
        // Выполняем цикл, который проверяет флаг прерывания потока
        while (!QThread::currentThread()->isInterruptionRequested())
        {
            longPoll_->start();  // блокирующий вызов, ожидает сообщение
        }
    }
    catch (const TgBot::TgException& e)
    {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
    emit finished();
}


void LongPollWorker::sendMessegeInTg(int64_t chatId, const std::string& message)
{
    try
    {
        bot_->getApi().sendMessage(chatId, message);
    }
    catch (const TgBot::TgException& e)
    {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

void LongPollWorker::sendPhotoInTg(int64_t chatId, const std::string& message, const std::string& mime)
{
    try
    {
        bot_->getApi().sendPhoto(chatId, TgBot::InputFile::fromFile(message, mime));
    }
    catch (const TgBot::TgException& e)
    {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}