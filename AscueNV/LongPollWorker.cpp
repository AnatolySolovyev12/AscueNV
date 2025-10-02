#include "LongPollWorker.h"
#include <QThread>
#include <qdebug.h>
#include <QCoreApplication.h>

LongPollWorker::LongPollWorker(QString any, QObject* parent)
    : QObject(parent), bot_(new TgBot::Bot(any.toStdString())), longPoll(new TgBot::TgLongPoll(*bot_, 90, 6))
{
    AttachConsole(ATTACH_PARENT_PROCESS);

    bot_->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        try {
            emit messageReceived(message);
        }
        catch (const std::exception& e) {
            qWarning() << "Error in message handler:" << e.what();
        }
        });

    qDebug() <<  bot_->getApi().getMe()->username.c_str();
}

LongPollWorker::~LongPollWorker()
{
}


void LongPollWorker::doLongPoll()
{
    try
    {
        while (!QThread::currentThread()->isInterruptionRequested() && !m_stopRequested)
        {
            QCoreApplication::processEvents();

            // Неблокирующий вызов longPoll с таймаутом
            QFuture<void> future = QtConcurrent::run([this]() { // запускаем пол и ждём завершения future в пуле потоков
                longPoll->start();
                });

            QFutureWatcher<void> watcher;
            watcher.setFuture(future); // связываем наблюдателя за завершением будущего события

            QEventLoop loop; // делаем петлю и ловим момент завершения future (polla) и если он не завершится то таймер прервёт петлю. 
            connect(&watcher, &QFutureWatcher<void>::finished, &loop, &QEventLoop::quit);
            QTimer::singleShot(10000, &loop, &QEventLoop::quit); // Таймаут 10 сек
            loop.exec();

            if (!future.isFinished()) { // если петля не завершится те сигналим о том что тут подвисло и всё это удаляется и строится заново иначе ресет и по новой
                // Прервать longPoll
                m_stopRequested = true;
                break;
            }

            emit resetWatchDogs();
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