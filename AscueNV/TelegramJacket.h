#pragma once

#include <QObject>
#include <stdio.h>
#include <tgbot/tgbot.h>

class TelegramJacket : QObject
{
	Q_OBJECT

public:
	TelegramJacket(QObject* parent = nullptr);
	~TelegramJacket();


private slots:
	void updateLongPoll();


private:

	TgBot::Bot* bot;

	TgBot::TgLongPoll* longPoll;

	TgBot::Message::Ptr * messageTest;



	QTimer* myTimer;




};
