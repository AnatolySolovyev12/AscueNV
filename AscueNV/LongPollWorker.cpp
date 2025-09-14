#include "LongPollWorker.h"
#include <QThread>
#include <qdebug.h>
#include <QCoreApplication.h>

LongPollWorker::LongPollWorker(TgBot::Bot* bot, QObject* parent)
    : QObject(parent), bot_(bot)
{
    bot_->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        try {
            emit messageReceived(message);
        }
        catch (const std::exception& e) {
            qWarning() << "Error in message handler:" << e.what();
        }
        });

    printf("Bot username: %s\n\n", bot_->getApi().getMe()->username.c_str());
}

LongPollWorker::~LongPollWorker()
{
    // Ничего не удаляем - используем автоматический объект
}

void LongPollWorker::doLongPoll()
{
    try
    {
        TgBot::TgLongPoll longPoll(*bot_, 25, 4);
        bool inProcessEvents = false; 

        while (!QThread::currentThread()->isInterruptionRequested())
        {
            longPoll.start();


            if (!inProcessEvents) {
                inProcessEvents = true;
                QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
                inProcessEvents = false;
            }
        }
    }
    catch (const TgBot::TgException& e)
    {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
    catch (const std::exception& e)
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