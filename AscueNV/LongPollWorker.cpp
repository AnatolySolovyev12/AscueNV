#include "LongPollWorker.h"
#include <QThread>
#include <qdebug.h>
#include <QCoreApplication.h>

LongPollWorker::LongPollWorker(QString any, QObject* parent)
	: QObject(parent), bot_(new TgBot::Bot(any.toStdString())), longPoll(new TgBot::TgLongPoll(*bot_, 90, 6)), pollTImer(new QTimer)
{
	qDebug() << bot_->getApi().getMe()->username.c_str();

	pollTImer->start(100);

	bot_->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
		try {
			emit messageReceived(message);
		}
		catch (const std::exception& e) {
			qWarning() << "Error in message handler:" << e.what();
		}
		});
}

LongPollWorker::~LongPollWorker()
{
}

void LongPollWorker::doLongPoll()
{
	connect(pollTImer, &QTimer::timeout, this, &LongPollWorker::timerPoll);
	pollTImer->setSingleShot(false); // Убедитесь, что таймер периодический
}


void LongPollWorker::timerPoll()
{
	if (QThread::currentThread()->isInterruptionRequested()) {
		return;
	}

	try {
		// Используйте неблокирующие вызовы или ограничьте время выполнения
		longPoll->start();

		// Обработайте события после каждого цикла
		QCoreApplication::processEvents();

		emit resetWatchDogs();
	}
	catch (const std::exception& e) {
		emit errorOccurred(QString::fromStdString(e.what()));
	}
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