#include <QtCore/QCoreApplication>
#include "DbTelegramExport.h"

#include <stdio.h>
#include <tgbot/tgbot.h>




int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);



	TgBot::Bot bot("7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM");




	bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {

		bot.getApi().sendMessage(message->chat->id, "Hi!");

		});

	bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {

		printf("User wrote %s\n", message->text.c_str());

		QString messegeString = message->text.c_str();

		messegeString = messegeString.trimmed();

		if (messegeString.length() < 6)
		{
			bot.getApi().sendMessage(message->chat->id, "Incorrect length. Need more");
			return;
		}

		if (messegeString.length() > 16)
		{
			bot.getApi().sendMessage(message->chat->id, "Incorrect length. Need less");
			return;
		}

		for (auto& val : messegeString)
		{
			if (val.isNumber())
				continue;


			bot.getApi().sendMessage(message->chat->id, "Incorrect symbol in number");
			return;
		}

		DbTelegramExport* forQuery = new DbTelegramExport();

		forQuery->setAny(messegeString);










		forQuery->queryDbResult(forQuery->getAny());

		//forQuery->removeAllConnection();

		if (StringTools::startsWith(message->text, "/start")) {
			return;
		}

		bot.getApi().sendMessage(message->chat->id, "Your message is: " + forQuery->getAny().toStdString() + "\n" + forQuery->getResult().toStdString());
		//bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);


		delete forQuery;
		forQuery = nullptr;

		});



	try {
		printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());

		TgBot::TgLongPoll longPoll(bot);

		while (true) {
			//printf("Long poll started\n");
			longPoll.start();
		}
	}
	catch (TgBot::TgException& e) {
		printf("error: %s\n", e.what());
	}


	return a.exec();


	//return 0;
}