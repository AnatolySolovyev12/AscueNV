#include <QtCore/QCoreApplication>
#include "DbTelegramExport.h"
#include "TelegramBotClass.h"

#include <qtimer.h>





int main(int argc, char* argv[])
{
	QCoreApplication a(argc, argv);

	TelegramBotClass myTgBot;

	QTimer::singleShot(5000, [&myTgBot]() {
		
		myTgBot.bot->getApi().sendMessage(myTgBot.messageTest->chat->id, "/start");
		
		});




	return a.exec();


	//return 0;
}