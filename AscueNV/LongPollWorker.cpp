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

            // ������������� ����� longPoll � ���������
            QFuture<void> future = QtConcurrent::run([this]() { // ��������� ��� � ��� ���������� future � ���� �������
                longPoll->start();
                });

            QFutureWatcher<void> watcher;
            watcher.setFuture(future); // ��������� ����������� �� ����������� �������� �������

            QEventLoop loop; // ������ ����� � ����� ������ ���������� future (polla) � ���� �� �� ���������� �� ������ ������ �����. 
            connect(&watcher, &QFutureWatcher<void>::finished, &loop, &QEventLoop::quit);
            QTimer::singleShot(10000, &loop, &QEventLoop::quit); // ������� 10 ���
            loop.exec();

            if (!future.isFinished()) { // ���� ����� �� ���������� �� �������� � ��� ��� ��� �������� � �� ��� ��������� � �������� ������ ����� ����� � �� �����
                // �������� longPoll
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