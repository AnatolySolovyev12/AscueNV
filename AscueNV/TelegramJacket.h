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

	//QString validation(std::string any);

private slots:
	void updateLongPoll();
	void setIntervalAfterGetString();

private:

	TgBot::Bot* bot;

	TgBot::TgLongPoll* longPoll;

	TgBot::Message::Ptr * messageTest;

	bool currentNeed = false;
	bool vecNeed = false;
	bool relayCounterOn = false;
	bool relayCounterOff = false;

	QTimer* myTimer = nullptr;

	QString messegeInTelegram;
	QString messegeFromTcp = "empty";
	QString ipFromDbTelegram;
	QString serialStringForProtocolinTelegram;

	DbTelegramExport* forQuery = nullptr;

	TcpClientForTelegram* tcpObj = nullptr;

	int counterForSlesh = 0;

	QList <QString> numberList{ "101", "102", "103", "104", "106", "/101", "/102", "/103" ,"/104", "/106", ">101", ">102", ">103" ,">104","_101", "_102", "_103", "_104", "*101", "*102", "*103", "*104", "*106"};

	int64_t myChat = 0;

	const std::string photoFilePath = "C:/Users/admin/source/repos/AscueNV/AscueNV/x64/Release/vectorP.jpg";
	const std::string photoMimeType = "image/jpeg";
};
