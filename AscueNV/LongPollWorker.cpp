#include "LongPollWorker.h"

LongPollWorker::LongPollWorker(TgBot::Bot* bot, QObject* parent)
    : QObject(parent)
    , bot_(bot)
{
    longPoll_ = new TgBot::TgLongPoll(*bot_, 100, 10);

    bot_->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
      //  emit messageReceived(message);
        });
}

LongPollWorker::~LongPollWorker()
{
    delete longPoll_;
}

void LongPollWorker::doLongPoll()
{
    try
    {
        while (true)
        {
            longPoll_->start();  // Блокирующий вызов, работает в своём потоке
        }
    }
    catch (TgBot::TgException& e)
    {
       // emit errorOccurred(QString::fromStdString(e.what()));
    }
}