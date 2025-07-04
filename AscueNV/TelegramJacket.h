#pragma once

#include <tgbot/tgbot.h>
#include <DbTelegramExport.h>
#include <TcpClientForTelegram.h>
#include <VectorImage.h>

#include <stdio.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QMainWindow>

#include <windows.h>

#include <qhash.h>
#include <qpair.h>

#include <QScopedPointer>

#include <qfile.h>

class TelegramJacket : public QMainWindow
{
	Q_OBJECT

public:
	TelegramJacket(QWidget* parent = nullptr);

private slots:
	void updateLongPoll();
	void setIntervalAfterGetString(const int64_t any);
	void setStopForVector();
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void cmdOpen();
	void cmdClose();
	void validChatIdInMassive();
	QString getTokenFromFile();
	
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

	//DbTelegramExport* forQuery = nullptr;

	TcpClientForTelegram* tcpObj = nullptr;

	int counterForSlesh = 0;

	QList <QString> numberList{ "101", "102", "103", "104", "106", "/101", "/102", "/103" ,"/104", "/106", ">101", ">102", ">103" ,">104","_101", "_102", "_103", "_104", "*101", "*102", "*103", "*104", "*106"};

	int64_t myChat = 0;

    const std::string photoFilePath = "_vectorP.png";
	const std::string photoMimeType = "image/png";

	VectorImage* editImage = nullptr;

	QSystemTrayIcon* trayIcon = nullptr;
	QDateTime fullTimeWork;

	QHash<int64_t, TcpClientForTelegram *>resultMassive;
	QHash<int64_t, VectorImage*>resultMassiveVector;
	QList<QString>chatIdMassive;
};
