#include "LongPollWorker.h"

LongPollWorker::LongPollWorker(TgBot::Bot* bot, QObject* parent)
    : QObject(parent)
    , bot_(bot), myTimer(new QTimer)
{
    longPoll_ = new TgBot::TgLongPoll(*bot_, 100, 5);

    bot_->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
      //  emit messageReceived(message);
        });

    connect(myTimer, SIGNAL(timeout()), this, SLOT(doLongPoll()));
    myTimer->setInterval(18000);
    myTimer->start();
}

LongPollWorker::~LongPollWorker()
{
    delete longPoll_;
}

void LongPollWorker::doLongPoll()
{
    try
    {
       // while (true)
       // {
            longPoll_->start();  // Блокирующий вызов, работает в своём потоке
       // }
    }
    catch (TgBot::TgException& e)
    {
       // emit errorOccurred(QString::fromStdString(e.what()));
    }
}