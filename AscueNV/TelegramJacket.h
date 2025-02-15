#pragma once

#include <stdio.h>
#include <tgbot/tgbot.h>
#include <DbTelegramExport.h>
#include <TcpClientForTelegram.h>
#include <VectorImage.h>
#include <qtimer.h>
#include <qdebug.h>

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMainWindow>

#include <windows.h>


class TelegramJacket : public QMainWindow
{
	Q_OBJECT

public:
	TelegramJacket(QWidget* parent = nullptr);

private slots:
	void updateLongPoll();
	void setIntervalAfterGetString();
	void setStopForVector();
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void cmdOpen();
	void cmdClose();
	
private:

	TgBot::Bot* bot;
	TgBot::TgLongPoll* longPoll;
	TgBot::Message::Ptr * messageTest;

	bool currentNeed = false;
	bool vecNeed = false;
	bool relayCounterOn = false;
	bool relayCounterOff = false;
	bool stopVector = false;

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

    const std::string photoFilePath = "mod_vectorP.png";
	const std::string photoMimeType = "image/png";

	VectorImage* editImage = nullptr;

	QSystemTrayIcon* trayIcon = nullptr;
	QTime fullTimeWork;
};
