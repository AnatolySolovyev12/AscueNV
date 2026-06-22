#include "LongPollWorker.h"
#include <QThread>
#include <qdebug.h>
#include <QCoreApplication.h>

LongPollWorker::LongPollWorker(QObject* parent)
	: QObject(parent), maxClass(new MaxClass)
{
	AttachConsole(ATTACH_PARENT_PROCESS);

	m_stopRequested = false;

	connect(maxClass, &MaxClass::lastMessageReceived, [this](const QPair<QString, QString>takePair) {

		auto msg = QSharedPointer<MyMessageObj>::create(
			takePair.first.toInt(),
			takePair.second.toStdString()
		);

		emit messageReceived(msg);

		});

	connect(this, &LongPollWorker::sendMessegeSignal, maxClass, &MaxClass::sendMessage);
	connect(this, &LongPollWorker::sendImageSignal, maxClass, &MaxClass::uploadFile);

	connect(maxClass, &MaxClass::finishNetworkAccessSignal, this, &LongPollWorker::doLongPoll);
}



void LongPollWorker::stopLongPoll()
{
	m_stopRequested = true;
}



void LongPollWorker::doLongPoll()
{
	if (!m_stopRequested)
	{
		QTimer::singleShot(3000, maxClass, &MaxClass::getLastMessageAsync);

		emit resetWatchDogs();
	}

	if (m_stopRequested)
		emit finished();
}



void LongPollWorker::sendMessegeInTg(int64_t chatId, const std::string& message)
{
	QString temp = QString::fromStdString(message);

	sendMessegeSignal(QString::number(chatId), temp);
}



void LongPollWorker::sendPhotoInTg(int64_t chatId, const std::string& message, const std::string& mime)
{
	sendImageSignal(QString::number(chatId), QString::fromStdString(message), QString::fromStdString(mime));
}