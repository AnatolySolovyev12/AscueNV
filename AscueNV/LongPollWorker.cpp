#include "LongPollWorker.h"
#include <QThread>
#include <qdebug.h>
#include <QCoreApplication.h>

LongPollWorker::LongPollWorker(QString any, QObject* parent)
    : QObject(parent), bot_(new TgBot::Bot(any.toStdString())), longPoll(new TgBot::TgLongPoll(*bot_, 90, 6))
{   
    qDebug() << bot_->getApi().getMe()->username.c_str();
}

LongPollWorker::~LongPollWorker()
{
}

void LongPollWorker::doLongPoll()
{
    bot_->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        try {
            emit messageReceived(message);
        }
        catch (const std::exception& e) {
            qWarning() << "Error in message handler:" << e.what();
        }
        });

    try
    {
        while (!QThread::currentThread()->isInterruptionRequested())
        {
            longPoll->start();

           // QCoreApplication::processEvents();

            emit resetWatchDogs();

            //if (m_stopRequested)
            //    break;
        }
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

void LongPollWorker::stopLongPoll()
{
    m_stopRequested = true;
}