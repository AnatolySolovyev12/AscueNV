#include "TelegramJacket.h"
#include <qtimer.h>
#include <qdebug.h>



TelegramJacket::TelegramJacket(QObject* parent)
	: QObject(parent)
{
	messageTest = new TgBot::Message::Ptr();

	myTimer = new QTimer();

	connect(myTimer, SIGNAL(timeout()), this, SLOT(updateLongPoll()));

	myTimer->setInterval(5000);

	myTimer->start();


	bot = new TgBot::Bot("7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM");


	bot->getEvents().onCommand("start", [this](TgBot::Message::Ptr messageTest) {

		bot->getApi().sendMessage(messageTest->chat->id, "Hi!");

		});


	bot->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {

		printf("User wrote %s\n", message->text.c_str());

		QString messegeString = message->text.c_str();

		messegeString = messegeString.trimmed();

		if (messegeString.length() < 6)
		{
			bot->getApi().sendMessage(message->chat->id, "Incorrect length. Need more");
			return;
		}

		if (messegeString.length() > 16)
		{
			bot->getApi().sendMessage(message->chat->id, "Incorrect length. Need less");
			return;
		}

		for (auto& val : messegeString)
		{
			if (val.isNumber())
				continue;


			bot->getApi().sendMessage(message->chat->id, "Incorrect symbol in number");
			return;
		}


		//DbTelegramExport* forQuery = new DbTelegramExport();

		//forQuery->setAny(messegeString);










		//forQuery->queryDbResult(forQuery->getAny());

		//forQuery->removeAllConnection();

		if (StringTools::startsWith(message->text, "/start")) {
			return;
		}

		//bot->getApi().sendMessage(message->chat->id, "Your message is: " + forQuery->getAny().toStdString() + "\n" + forQuery->getResult().toStdString());
		bot->getApi().sendMessage(message->chat->id, "Your message is: " + message->text);


		//delete forQuery;
		//forQuery = nullptr;

		});


	longPoll = new TgBot::TgLongPoll(*bot);

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

void TelegramJacket::updateLongPoll()
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




