#include <QtCore/QCoreApplication>
#include "DbTelegramExport.h"

#include <stdio.h>
#include <tgbot/tgbot.h>




int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);



    TgBot::Bot bot("7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM");


    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) 
        {
        bot.getApi().sendMessage(message->chat->id, "Julia, i love you!");
        });


    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        



        printf("User wrote %s\n", message->text.c_str());

        DbTelegramExport* forQuery = new DbTelegramExport();

        forQuery->setAny(message->text.c_str());
      //  qDebug() << forQuery->getAny();

        forQuery->queryDbResult(forQuery->getAny());

        //qDebug() << forQuery->getAny();

        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }

        bot.getApi().sendMessage(message->chat->id, "Your message is: " + forQuery->getAny().toStdString() + "\n" + forQuery->getResult().toStdString());
        //bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);


        delete forQuery;

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