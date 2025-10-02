#include "TelegramJacket.h"

TelegramJacket::TelegramJacket(QWidget* parent)
	: QMainWindow(parent), destructionAndResurecctionTimer(new QTimer)
{
	setupLongPoll();

	fullTimeWork = QDateTime::currentDateTime();

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(QCoreApplication::applicationDirPath() + "\\icon.png"));

	validChatIdInMassive();

	QMenu* menu = new QMenu(this);
	QAction* restoreAction = menu->addAction("CMD open and connect");
	QAction* restoreActionHide = menu->addAction("CMD disconnect");
	QAction* quitAction = menu->addAction("Exit");

	connect(restoreAction, &QAction::triggered, this, &TelegramJacket::cmdOpen);
	connect(restoreActionHide, &QAction::triggered, this, &TelegramJacket::cmdClose);
	connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

	trayIcon->setContextMenu(menu);
	trayIcon->setVisible(true);

	connect(trayIcon, &QSystemTrayIcon::activated, this, &TelegramJacket::iconActivated);

	restartWatchDogs();
	
	connect(destructionAndResurecctionTimer, &QTimer::timeout, this, &TelegramJacket::restartLongPoll);
}

void TelegramJacket::restartLongPoll()
{
	destructionAndResurecctionTimer->stop();

	if (longPollThread && longPollThread->isRunning()) {

		emit stopNetworkConnectionSignal(); // сигнал для принудительной остановки LongPoll

		longPollThread->requestInterruption();

		connect(longPollThread, &QThread::finished, this, [this]() { // по сигналу об остановке потока удаляем его и вложенный объект телеги
			disconnect(longPollThread, nullptr, nullptr, nullptr);
			disconnect(longPollWorker, nullptr, nullptr, nullptr);
			longPollWorker->deleteLater();
			longPollThread->deleteLater();
			longPollWorker = nullptr;
			longPollThread = nullptr;

			setupLongPoll(); // пересоздаём все эти объекты
			restartWatchDogs(); //делаем ребут
			});

		longPollThread->quit();
		longPollThread->wait(); // дождаться завершения
	}
	else {
		// Если поток не запущен, просто пересоздаем
		setupLongPoll();
		restartWatchDogs();
	}

	qDebug() << "\n" << QDate::currentDate().toString("dd.MM.yyyy") + " - " + QTime::currentTime().toString() + " was ressurection" << "\n";

	writeMessegeHistory(" was ressurection");
}


void TelegramJacket::setupLongPoll()
{
	longPollWorker = new LongPollWorker(getTokenFromFile());
	longPollThread = new QThread(this);
	longPollWorker->moveToThread(longPollThread);

	connect(longPollThread, &QThread::started, longPollWorker, &LongPollWorker::doLongPoll, Qt::UniqueConnection); // после старта потока сигналим на запуск LongPoll

	connect(longPollWorker, &LongPollWorker::messageReceived, this, &TelegramJacket::onMessageReceived, Qt::UniqueConnection); // приём сообщений из бота
	connect(this, &TelegramJacket::sendMessageRequested, longPollWorker, &LongPollWorker::sendMessegeInTg, Qt::UniqueConnection); // отправка сигнала с сообщением в бота который в отбельном потоке
	connect(this, &TelegramJacket::sendVectorPhoto, longPollWorker, &LongPollWorker::sendPhotoInTg, Qt::UniqueConnection); // отправка сигнала с сообщением в бота который в отбельном потоке
	connect(longPollWorker, &LongPollWorker::resetWatchDogs, this, &TelegramJacket::restartWatchDogs, Qt::UniqueConnection);
	connect(this, &TelegramJacket::stopNetworkConnectionSignal, longPollWorker, &LongPollWorker::stopLongPoll, Qt::UniqueConnection);
	connect(longPollWorker, &LongPollWorker::errorOccurred, this, &TelegramJacket::writeMessegeHistory, Qt::UniqueConnection);

	longPollThread->start();
}


void TelegramJacket::restartWatchDogs()
{
	destructionAndResurecctionTimer->stop();
	destructionAndResurecctionTimer->start(300000);
}


void TelegramJacket::setIntervalAfterGetString(const int64_t any) // автовывод сообщения после получения текущих от счётчика
{
	serialStringForProtocolinTelegram = resultMassive.find(any).value()->getSerialStringForProtocol();

	if ((serialStringForProtocolinTelegram == "*102" || serialStringForProtocolinTelegram == "*104" || serialStringForProtocolinTelegram == "*106") && !stopVector)
	{
		if (resultMassiveVector.find(any) != resultMassiveVector.constEnd())
		{
			delete resultMassiveVector.find(any).value();
			resultMassiveVector.find(any).value() = nullptr;
			resultMassiveVector.find(any).value() = new VectorImage(this);
			resultMassiveVector.find(any).value()->setKey(any);
			resultMassiveVector.find(any).value()->generalFunc(resultMassive.find(any).value()->returnResultString());
			emit sendVectorPhoto(any, (QString::number(any).toStdString() + photoFilePath), photoMimeType);
		}
		else
		{
			resultMassiveVector.insert(any, new VectorImage(this));
			resultMassiveVector.find(any).value()->setKey(any);
			resultMassiveVector.find(any).value()->generalFunc(resultMassive.find(any).value()->returnResultString());
			emit sendVectorPhoto(any, (QString::number(any).toStdString() + photoFilePath), photoMimeType);
		}
	}

	emit sendMessageRequested(any, resultMassive.find(any).value()->returnResultString().toStdString());

	stopVector = false;
}


void TelegramJacket::setStopForVector() // автовывод сообщения после получения текущих от счётчика
{
	stopVector = true;
}


void TelegramJacket::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::ActivationReason::DoubleClick) // требуется корректировка вывода часов переведённых в сутки или в часах свыше 24
	{
		qint64 test = fullTimeWork.secsTo(QDateTime::currentDateTime());
		QString days = QString::number(test / 86400);
		trayIcon->showMessage("All time from start:", "Days " + days + " Time " + QTime(0, 0, 0).addSecs(test % 86400).toString() + " rm " + QString::number(resultMassive.size()), QSystemTrayIcon::Information, 5000);
	}
}


void TelegramJacket::cmdOpen()
{
	AllocConsole(); // Создаем консоль и присоединяем к ней текущий процесс
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout); // Перенаправляем стандартный вывод
	freopen_s(&stream, "CONOUT$", "w", stderr); // Перенаправляем стандартный вывод ошибок
}


void TelegramJacket::cmdClose()
{
	qDebug() << "\nProgramm disconnect from console.";

	FreeConsole(); // Отделяем процесс от cmd. После cmd закрываем руками.
}


void TelegramJacket::validChatIdInMassive()
{
	QFile file(QCoreApplication::applicationDirPath() + "\\chatIdMassive.txt");

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Error in TelegramJacket::validChatIdInMassive(not find file or other):" << file.error();
	}

	QString line;

	QTextStream in(&file);

	while (!in.atEnd())
	{
		line = in.readLine(12);

		if (chatIdMassive.length() > 20)
		{
			qDebug() << "Error: Max length for chatIdMassive is 20";
			break;
		}

		chatIdMassive.push_back(line);
	}

	file.close();
}


QString TelegramJacket::getTokenFromFile()
{
	QFile file(QCoreApplication::applicationDirPath() + "\\token.txt");

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Don't find browse file. Add a directory with a token (token.txt).";
		return 0;
	}

	QTextStream out(&file);

	QString myLine = out.readLine(); // метод readLine() считывает одну строку из потока

	if (myLine == "")
	{
		qDebug() << "Don't find browse file. Add a directory with a token (token.txt).";
		file.close();
		return 0;
	}

	file.close();

	return myLine;
}


void TelegramJacket::writeMessegeHistory(QString any)
{
	QString filename = QCoreApplication::applicationDirPath() + "\\history.txt";
	QFile file(filename);

	// Открываем файл в режиме "Только для записи и дополнения без перезаписи"
	if (file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream out(&file); // поток записываемых данных направляем в файл
		out << QDate::currentDate().toString("dd.MM.yyyy") + " - " + QTime::currentTime().toString() + " - " + any << Qt::endl;
	}
	else
	{
		qWarning("Could not open file");
	}

	file.close();
}


void TelegramJacket::onMessageReceived(TgBot::Message::Ptr message)
{
	messegeInTelegram = message->text.c_str();

	qDebug() << messegeInTelegram;

	writeMessegeHistory(QString::number(message->chat->id) + " - " + messegeInTelegram);

	if (messegeInTelegram == "/start")
	{
		emit sendMessageRequested(message->chat->id, "Your ChatID: " + QString::number(message->chat->id).toStdString() + "\n<serial> - last daily and connection parameters\n</serial> - current values\n<*serial> - vector and identifications\n<_serial> - relay on\n<>serial> - relay off");
		myChat = message->chat->id;

		return;
	}

	if (messegeInTelegram == "/result")
	{
		if (resultMassive.find(message->chat->id) != resultMassive.constEnd())
		{
			//messegeFromTcp = tcpObj->returnResultString();

			if (resultMassive.find(message->chat->id).value()->returnResultString().toStdString() == "")
			{
				emit sendMessageRequested(message->chat->id, "empty");
			}
			else
			{
				emit sendMessageRequested(message->chat->id, resultMassive.find(message->chat->id).value()->returnResultString().toStdString());
			}
		}
		else
		{
			emit sendMessageRequested(message->chat->id, "empty");
		}
		return;
	}

	if (messegeInTelegram.length() < 6) // Validation messege
	{
		messegeInTelegram = "";
		emit sendMessageRequested(message->chat->id, "Incorrect length.Need more");
		return;
	}

	if (messegeInTelegram.length() > 16) // Validation messege
	{
		messegeInTelegram = "";
		emit sendMessageRequested(message->chat->id, "Incorrect length. Need less");
		return;
	}

	for (auto& val : messegeInTelegram) // Validation messege
	{
		if ((val == '_' || val == '>') && counterForSlesh == 0)
		{
			if (chatIdMassive.indexOf(QString::number(message->chat->id)) == -1)
			{
				emit sendMessageRequested(message->chat->id, "Access for this command is not for you (_,_)");
				return;
			}

			if (val == '_')
				relayCounterOn = true;
			else
				relayCounterOff = true;

			counterForSlesh++;
			continue;
		}

		if ((val == '/' || val == '*') && counterForSlesh == 0)
		{
			if (val == '/')
				currentNeed = true;
			else
				vecNeed = true;
			counterForSlesh++;
			continue;
		}

		if (val.isNumber())
			continue;

		messegeInTelegram = "";
		currentNeed = false;
		vecNeed = false;
		relayCounterOn = false;
		relayCounterOff = false;
		emit sendMessageRequested(message->chat->id, "Incorrect symbol in number");
		return;
	}

	counterForSlesh = 0;

	QScopedPointer<DbTelegramExport>forQuery(new DbTelegramExport);

	if (currentNeed || vecNeed || relayCounterOn || relayCounterOff)
	{
		messegeInTelegram = messegeInTelegram.sliced(1);
		forQuery->setAny(messegeInTelegram);
	}
	else
	{
		forQuery->setAny(messegeInTelegram);
	}

	forQuery->queryDbResult(forQuery->getAny());

	if ((currentNeed || vecNeed) && (messegeInTelegram != ""))
	{
		for (auto& val : forQuery->getIpForTcp())
		{
			if (val == ':') break;
			ipFromDbTelegram += val;
		}

		if (ipFromDbTelegram != "")
		{
			int count = 0;
			serialStringForProtocolinTelegram = "";

			for (auto& val : messegeInTelegram)
			{
				if (count == 3) break;
				serialStringForProtocolinTelegram += val;
				++count;
			}

			if (vecNeed)
				serialStringForProtocolinTelegram.push_front('*');

			if (numberList.indexOf(serialStringForProtocolinTelegram) >= 0)
			{
				if (resultMassive.find(message->chat->id) != resultMassive.constEnd())
				{
					delete resultMassive.find(message->chat->id).value();
					resultMassive.find(message->chat->id).value() = nullptr;
					resultMassive.find(message->chat->id).value() = new TcpClientForTelegram(serialStringForProtocolinTelegram);
					resultMassive.find(message->chat->id).value()->setKey(message->chat->id);
					resultMassive.find(message->chat->id).value()->setResultString(messegeInTelegram);

					QObject::connect(resultMassive.find(message->chat->id).value(), SIGNAL(messageReceived(int64_t)), this, SLOT(setIntervalAfterGetString(int64_t))); // connect для автовывода сообщения в чат после опроса текущих
					QObject::connect(resultMassive.find(message->chat->id).value(), SIGNAL(messageError()), this, SLOT(setStopForVector())); // сигнал с ошибкой чтобы не выводить векторную диаграмму
				}
				else
				{
					resultMassive.insert(message->chat->id, new TcpClientForTelegram(serialStringForProtocolinTelegram));
					resultMassive.find(message->chat->id).value()->setKey(message->chat->id);
					resultMassive.find(message->chat->id).value()->setResultString(messegeInTelegram);

					QObject::connect(resultMassive.find(message->chat->id).value(), SIGNAL(messageReceived(int64_t)), this, SLOT(setIntervalAfterGetString(int64_t)));  // connect для автовывода сообщения в чат после опроса текущих
					QObject::connect(resultMassive.find(message->chat->id).value(), SIGNAL(messageError()), this, SLOT(setStopForVector())); // сигнал с ошибкой чтобы не выводить векторную диаграмму
				}

				if (currentNeed)
					emit sendMessageRequested(message->chat->id, "We started trying to get current values ​​from the device " + forQuery->getAny().toStdString() + ". Wait a 2-3 minute and you get a messege. Also you can get current if you send: /result. Repeat if it needed.");
				else
					emit sendMessageRequested(message->chat->id, "We started trying to get vector and identification parameters ​​from the device " + forQuery->getAny().toStdString() + ". Wait a 1-2 minute and you get a messege. Also you can get these if you send: /result. Repeat if it needed.");

				resultMassive.find(message->chat->id).value()->startToConnect(ipFromDbTelegram);
				ipFromDbTelegram = "";
			}
			else
			{
				emit sendMessageRequested(message->chat->id, "Incorrect device for this command");
			}
		}
		else
		{
			emit sendMessageRequested(message->chat->id, "Not found ip adress for this device. Check your number and try again");
		}
	}

	if ((relayCounterOn || relayCounterOff) && (messegeInTelegram != ""))
	{
		for (auto& val : forQuery->getIpForTcp())
		{
			if (val == ':') break;
			ipFromDbTelegram += val;
		}

		if (ipFromDbTelegram != "")
		{
			int count = 0;
			serialStringForProtocolinTelegram = "";

			for (auto& val : messegeInTelegram)
			{
				if (count == 3)
				{
					if (relayCounterOn)
						serialStringForProtocolinTelegram.push_front('_');
					else
						serialStringForProtocolinTelegram.push_front('>');
					break;
				}
				serialStringForProtocolinTelegram += val;
				++count;
			}

			if (numberList.indexOf(serialStringForProtocolinTelegram) >= 0)
			{
				if (resultMassive.find(message->chat->id) != resultMassive.constEnd())
				{
					delete resultMassive.find(message->chat->id).value();
					resultMassive.find(message->chat->id).value() = nullptr;
					resultMassive.find(message->chat->id).value() = new TcpClientForTelegram(serialStringForProtocolinTelegram);
					resultMassive.find(message->chat->id).value()->setKey(message->chat->id);
					resultMassive.find(message->chat->id).value()->setResultString(messegeInTelegram);

					QObject::connect(resultMassive.find(message->chat->id).value(), SIGNAL(messageReceived(int64_t)), this, SLOT(setIntervalAfterGetString(int64_t))); // connect для автовывода сообщения в чат после опроса текущих
					QObject::connect(resultMassive.find(message->chat->id).value(), SIGNAL(messageError()), this, SLOT(setStopForVector())); // сигнал с ошибкой чтобы не выводить векторную диаграмму
				}
				else
				{
					resultMassive.insert(message->chat->id, new TcpClientForTelegram(serialStringForProtocolinTelegram));
					resultMassive.find(message->chat->id).value()->setKey(message->chat->id);
					resultMassive.find(message->chat->id).value()->setResultString(messegeInTelegram);

					QObject::connect(resultMassive.find(message->chat->id).value(), SIGNAL(messageReceived(int64_t)), this, SLOT(setIntervalAfterGetString(int64_t)));  // connect для автовывода сообщения в чат после опроса текущих
					QObject::connect(resultMassive.find(message->chat->id).value(), SIGNAL(messageError()), this, SLOT(setStopForVector())); // сигнал с ошибкой чтобы не выводить векторную диаграмму
				}

				if (relayCounterOn)
					emit sendMessageRequested(message->chat->id, "We started trying to connect relay ​​for device " + forQuery->getAny().toStdString() + ". Wait a 2-3 minute and you get a messege. Also you can get current if you send: /result. Repeat if it needed.");
				else
					emit sendMessageRequested(message->chat->id, "We started trying to disconnect relay ​​for device " + forQuery->getAny().toStdString() + ". Wait a 2-3 minute and you get a messege. Also you can get current if you send: /result. Repeat if it needed.");

				resultMassive.find(message->chat->id).value()->startToConnect(ipFromDbTelegram);
				ipFromDbTelegram = "";
			}
			else
			{
				emit sendMessageRequested(message->chat->id, "Incorrect device for this command");
			}
		}
		else
		{
			emit sendMessageRequested(message->chat->id, "Not found ip adress for this device. Check your number and try again");
		}
	}

	if (!currentNeed && !relayCounterOn && !relayCounterOff && !vecNeed)
	{
		emit sendMessageRequested(message->chat->id, "Your message is: " + forQuery->getAny().toStdString() + "\n" + forQuery->getResult().toStdString());
	}
	currentNeed = false;
	relayCounterOn = false;
	relayCounterOff = false;
	vecNeed = false;
	messegeInTelegram = "";

	if (StringTools::startsWith(message->text, "/start")) {
		return;
	}
}
