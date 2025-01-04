#include "TelegramJacket.h"
#include <qtimer.h>
#include <qdebug.h>


TelegramJacket::TelegramJacket(QObject* parent)
	: QObject(parent)
{
	bot = new TgBot::Bot("7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM");

	messageTest = new TgBot::Message::Ptr();

	longPoll = new TgBot::TgLongPoll(*bot, 100, 5); // если timeout = 3 то пишет мусор в result-ную строку TcpCLient-а. Видимо чего то не успевает.

	myTimer = new QTimer();

	connect(myTimer, SIGNAL(timeout()), this, SLOT(updateLongPoll()));
	myTimer->setInterval(1000);
	myTimer->start();

	bot->getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {

		bot->getApi().sendMessage(message->chat->id, "<serial number> - some parameters\n</><serial number> - current values\n<_><serial number> - relay on\n<>><serial number> - relay off");
		myChat = message->chat->id;
		});

	bot->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {

		printf("User wrote %s\n", message->text.c_str());

		//messegeInTelegram = validation(message->text.c_str());

		messegeInTelegram = message->text.c_str();

		if (messegeInTelegram == "/start")
		{
			return;
		}

		if (messegeInTelegram == "/result")
		{
			if (tcpObj != nullptr)
			{
				messegeFromTcp = tcpObj->returnResultString();
			}
			if (messegeFromTcp == "") messegeFromTcp = "empty";
			//messegeFromTcp = "empty";
			bot->getApi().sendMessage(message->chat->id, messegeFromTcp.toStdString());
			//messegeFromTcp = "empty";
			//tcpObj->resetAnswerString();
			return;
		}

		if (messegeInTelegram.length() < 6)
		{
			messegeInTelegram = "";
			bot->getApi().sendMessage(message->chat->id, "Incorrect length.Need more");
			return;
		}

		if (messegeInTelegram.length() > 16)
		{
			messegeInTelegram = "";
			bot->getApi().sendMessage(message->chat->id, "Incorrect length. Need less");
			return;
		}

		for (auto& val : messegeInTelegram)
		{
			if ((val == '_' || val == '>') && counterForSlesh == 0) /// надо рихтовать с палками
			{
				if (val == '_')
					relayCounterOn = true;
				else
					relayCounterOff = true;

				counterForSlesh++;
				continue;
			}

			if (val == '/' && counterForSlesh == 0) /// надо рихтовать с палками
			{
				currentNeed = true;
				counterForSlesh++;
				continue;
			}

			if (val.isNumber())
				continue;

			messegeInTelegram = "";
			currentNeed = false;
			relayCounterOn = false;
			relayCounterOff = false;
			bot->getApi().sendMessage(message->chat->id, "Incorrect symbol in number");
			return;
		}

		counterForSlesh = 0;

		forQuery = new DbTelegramExport();

		//myTimer->setInterval(100000);
		if (currentNeed || relayCounterOn || relayCounterOff)
		{
			messegeInTelegram = messegeInTelegram.sliced(1);
			forQuery->setAny(messegeInTelegram);
		}
		else
		{
			forQuery->setAny(messegeInTelegram); ////////////вероятно лишняя возня. Стоит оптимизировать
		}

		forQuery->queryDbResult(forQuery->getAny()); ////////////вероятно лишняя возня. СТоит оптимизировать

		if (currentNeed && (messegeInTelegram != ""))
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

				if (numberList.indexOf(serialStringForProtocolinTelegram) >= 0)
				{
					myChat = message->chat->id; // фиксируем id чата для автовывода сообщения после опроса текущих

					delete tcpObj;
					tcpObj = nullptr;

					tcpObj = new TcpClientForTelegram(serialStringForProtocolinTelegram);

					QObject::connect(tcpObj, SIGNAL(messageReceived()), this, SLOT(setIntervalAfterGetString())); // connect для автовывода сообщения в чат после опроса текущих

					messegeInTelegram += '\n';
					tcpObj->setResultString(messegeInTelegram);

					bot->getApi().sendMessage(message->chat->id, "We started trying to get current values ​​from the device " + forQuery->getAny().toStdString() + ". Wait a 2-3 minute and you get a messege. Also you can get current if you send: /result. Repeat if it needed.");

					tcpObj->startToConnect(ipFromDbTelegram);
					ipFromDbTelegram = "";
				}
				else
				{
					bot->getApi().sendMessage(message->chat->id, "Incorrect device for this command");
				}
			}
			else
			{
				bot->getApi().sendMessage(message->chat->id, "Not found ip adress for this device. Check your number and try again");
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
					myChat = message->chat->id;

					delete tcpObj;
					tcpObj = nullptr;

					tcpObj = new TcpClientForTelegram(serialStringForProtocolinTelegram);

					QObject::connect(tcpObj, SIGNAL(messageReceived()), this, SLOT(setIntervalAfterGetString())); // connect для автовывода сообщения в чат после опроса текущих

					if (relayCounterOn)
						bot->getApi().sendMessage(message->chat->id, "We started trying to connect relay ​​for device " + forQuery->getAny().toStdString() + ". Wait a 2-3 minute and you get a messege. Also you can get current if you send: /result. Repeat if it needed.");
					else
						bot->getApi().sendMessage(message->chat->id, "We started trying to disconnect relay ​​for device " + forQuery->getAny().toStdString() + ". Wait a 2-3 minute and you get a messege. Also you can get current if you send: /result. Repeat if it needed.");

					tcpObj->startToConnect(ipFromDbTelegram);
					ipFromDbTelegram = "";
				}
				else
				{
					bot->getApi().sendMessage(message->chat->id, "Incorrect device for this command");
				}
			}
			else
			{
				bot->getApi().sendMessage(message->chat->id, "Not found ip adress for this device. Check your number and try again");
			}
		}












		if (!currentNeed && !relayCounterOn && !relayCounterOff)
		{
			bot->getApi().sendMessage(message->chat->id, "Your message is: " + forQuery->getAny().toStdString() + "\n" + forQuery->getResult().toStdString());
		}
		currentNeed = false;
		relayCounterOn = false;
		relayCounterOff = false;
		messegeInTelegram = "";

		//bot->getApi().sendMessage(message->chat->id, "Your message is: " + message->text);

		delete forQuery;
		forQuery = nullptr;




		if (StringTools::startsWith(message->text, "/start")) {
			return;
		}
		});








	try {
		printf("Bot username: %s\n\n", bot->getApi().getMe()->username.c_str());
		TgBot::TgLongPoll longPoll(*bot);

		/*
		while (true) {
			//printf("Long poll started\n");
			longPoll.start();
		}
		*/
	}
	catch (TgBot::TgException& e) {
		printf("error: %s\n", e.what());
	}
}





void TelegramJacket::setIntervalAfterGetString() // автовывод сообщения после получения текущих от счётчика
{
	messegeFromTcp = tcpObj->returnResultString();
	bot->getApi().sendMessage(myChat, messegeFromTcp.toStdString());
}

/*
QString TelegramJacket::validation(std::string any)   // Пока не требуется //////////////////////////////////////
{
	QString messegeString = QString::fromStdString(any);

	messegeString = messegeString.trimmed();

	if (messegeString.length() < 6)
	{
		return "Incorrect length. Need more";
	}

	if (messegeString.length() > 16)
	{
		return "Incorrect length. Need less";
	}

	for (auto& val : messegeString)
	{
		if (val.isNumber())
			continue;

		return "Incorrect symbol in number";
	}

	return messegeString;
}
*/

void TelegramJacket::updateLongPoll() // обновляем longPoll за счёт периодического таймера
{
	try {

		//qDebug() << "test longPoll with timer";
		longPoll->start();
	}
	catch (TgBot::TgException& e) {
		printf("error: %s\n", e.what());
	}

}

TelegramJacket::~TelegramJacket()
{}




