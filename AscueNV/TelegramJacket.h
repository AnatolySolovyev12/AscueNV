#pragma once

#include <QObject>
#include <stdio.h>
#include <tgbot/tgbot.h>
#include <DbTelegramExport.h>
#include <TcpClientForTelegram.h>

class TelegramJacket : QObject
{
	Q_OBJECT

public:
	TelegramJacket(QObject* parent = nullptr);
	~TelegramJacket();


	QString validation(std::string any);




private slots:
	void updateLongPoll();
	void setIntervalAfterGetString(QString);


private:

	TgBot::Bot* bot;

	TgBot::TgLongPoll* longPoll;

	TgBot::Message::Ptr * messageTest;

	bool currentNeed = false;

	QTimer* myTimer = nullptr;

	QString messegeInTelegram;
	QString messegeFromTcp = "empty";
	QString ipFromDbTelegram;

	DbTelegramExport* forQuery = nullptr;

	TcpClientForTelegram* tcpObj = nullptr;

	int counterForSlesh = 0;



};
