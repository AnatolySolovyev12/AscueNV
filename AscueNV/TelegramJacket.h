#pragma once

#include <QObject>
#include <stdio.h>
#include <tgbot/tgbot.h>
#include <DbTelegramExport.h>

class TelegramJacket : QObject
{
	Q_OBJECT

public:
	TelegramJacket(QObject* parent = nullptr);
	~TelegramJacket();


	QString validation(std::string any);




private slots:
	void updateLongPoll();


private:

	TgBot::Bot* bot;

	TgBot::TgLongPoll* longPoll;

	TgBot::Message::Ptr * messageTest;



	QTimer* myTimer = nullptr;;

	DbTelegramExport* forQuery = nullptr;

	QString messegeInTelegram;



};
