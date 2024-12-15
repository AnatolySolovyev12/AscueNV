#include "TelegramJacket.h"
#include <qtimer.h>
#include <qdebug.h>



TelegramJacket::TelegramJacket(QObject* parent)
	: QObject(parent)
{
	bot = new TgBot::Bot("7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM");

	messageTest = new TgBot::Message::Ptr();

	longPoll = new TgBot::TgLongPoll(*bot, 100, 4); // если timeout = 3 то пишет мусор в result-ную строку TcpCLient-а. Видимо чего то не успевает.


	myTimer = new QTimer();


	connect(myTimer, SIGNAL(timeout()), this, SLOT(updateLongPoll()));
	myTimer->setInterval(1500);
	myTimer->start();

	

	

	

	bot->getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {

		bot->getApi().sendMessage(message->chat->id, "Hi!");

		});

	/*
	bot->getEvents().onCommand("result", [this](TgBot::Message::Ptr message) {

		messegeFromTcp = tcpObj->returnResultString();
		bot->getApi().sendMessage(message->chat->id, messegeFromTcp.toStdString());
		messegeFromTcp = "";

		});
    */

	bot->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {

		printf("User wrote %s\n", message->text.c_str());

		//messegeInTelegram = validation(message->text.c_str());

		messegeInTelegram = message->text.c_str();
		
		if (messegeInTelegram == "/result")
		{
			messegeFromTcp = tcpObj->returnResultString();
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
			return ;
		}

		for (auto& val : messegeInTelegram)
		{
			

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
			bot->getApi().sendMessage(message->chat->id, "Incorrect symbol in number");
			return ;
		}
		
		counterForSlesh = 0;

		forQuery = new DbTelegramExport();

		//myTimer->setInterval(100000);
		if (currentNeed)
		{
			messegeInTelegram = messegeInTelegram.sliced(1);
			forQuery->setAny(messegeInTelegram);
		}
		else
		{
			forQuery->setAny(messegeInTelegram);
		}

		forQuery->queryDbResult(forQuery->getAny());



		if (currentNeed && (messegeInTelegram != ""))
		{
			for (auto& val : forQuery->getIpForTcp())
			{
				if (val == ':') break;
				ipFromDbTelegram += val;
			}

			delete tcpObj;
			tcpObj = nullptr;

			tcpObj = new TcpClientForTelegram();
			tcpObj->startToConnect(ipFromDbTelegram); // добавить проверку на пустой IP
			currentNeed = false;
			ipFromDbTelegram = "";
		}
		

		
		//connect(tcpObj, SIGNAL(status(QString)), this, SLOT(setIntervalAfterGetString(QString)));// прям в методе после создания объекта

		bot->getApi().sendMessage(message->chat->id, "Your message is: " + forQuery->getAny().toStdString() + "\n" + forQuery->getResult().toStdString());
		
		messegeInTelegram = "";
		
		//bot->getApi().sendMessage(message->chat->id, "Your message is: " + message->text);

		delete forQuery;
		forQuery = nullptr;




		if (StringTools::startsWith(message->text, "/start")) {
			return;
		}
		});








	try {
		printf("Bot username: %s\n", bot->getApi().getMe()->username.c_str());
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





void TelegramJacket::setIntervalAfterGetString(QString) // пока не требуется /////////////////////////////////////
{
	myTimer->setInterval(7000);
	//messegeFromTcp = any;
}

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

void TelegramJacket::updateLongPoll() // обновляем longPoll за счёт периодического таймера
{
	try {

		qDebug() << "test longPoll with timer";
		longPoll->start();
	}
	catch (TgBot::TgException& e) {
		printf("error: %s\n", e.what());
	}

}

TelegramJacket::~TelegramJacket()
{}




