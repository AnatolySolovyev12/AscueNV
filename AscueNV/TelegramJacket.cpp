#include "TelegramJacket.h"
#include <qtimer.h>
#include <qdebug.h>



TelegramJacket::TelegramJacket(QObject* parent)
	: QObject(parent)
{
	messageTest = new TgBot::Message::Ptr();

	myTimer = new QTimer();
	connect(myTimer, SIGNAL(timeout()), this, SLOT(updateLongPoll()));
	myTimer->setInterval(7000);
	myTimer->start();

	forQuery = new DbTelegramExport();

	bot = new TgBot::Bot("7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM");

	longPoll = new TgBot::TgLongPoll(*bot);








	bot->getEvents().onCommand("start", [this](TgBot::Message::Ptr message) {

		bot->getApi().sendMessage(message->chat->id, "Hi!");

		});


	bot->getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {

		printf("User wrote %s\n", message->text.c_str());

		messegeInTelegram = validation(message->text.c_str());
		




		forQuery->setAny(messegeInTelegram);

		forQuery->queryDbResult(forQuery->getAny());

		//forQuery->removeAllConnection();



		bot->getApi().sendMessage(message->chat->id, "Your message is: " + forQuery->getAny().toStdString() + "\n" + forQuery->getResult().toStdString());
		//bot->getApi().sendMessage(message->chat->id, "Your message is: " + message->text);


		//delete forQuery;
		//forQuery = nullptr;




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








QString TelegramJacket::validation(std::string any)
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




