#pragma once

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
#include <shellapi.h>
#include <qhash.h>
#include <qpair.h>
#include <QScopedPointer>
#include <qfile.h>
#include <LongPollWorker.h>
#include <QThread.h>
#include <MessegeStruct.h>

class TelegramJacket : public QMainWindow
{
	Q_OBJECT

public:
	TelegramJacket(QWidget* parent = nullptr);
	~TelegramJacket();

private slots:
	void setIntervalAfterGetString(const int64_t any);
	void setStopForVector();
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void cmdOpen();
	void cmdClose();
	void validChatIdInMassive();
	QString getTokenFromFile();
	void writeMessegeHistory(QString any);
	void onMessageReceived(QSharedPointer<MyMessageObj>message);

	void restartWatchDogs();
	void restartLongPoll();
	void setupLongPoll();

signals:
	void sendMessageRequested(int64_t chatId, const std::string& message);
signals:
	void sendVectorPhoto(int64_t chatId, const std::string& message, const std::string& mime);
signals:
	void stopNetworkConnectionSignal();
	
private:
	bool currentNeed = false;
	bool vecNeed = false;
	bool relayCounterOn = false;
	bool relayCounterOff = false;
	bool stopVector = false;
	bool dailyArchiveBool = false;

	QTimer* myTimer = nullptr;

	QString messegeInTelegram;
	QString messegeFromTcp = "empty";
	QString ipFromDbTelegram;
	QString serialStringForProtocolinTelegram;
	QString dailyArchiveString = "";

	TcpClientForTelegram* tcpObj = nullptr;

	int counterForSlesh = 0;

	QList <QString> numberList{ "101", "102", "103", "104", "106", "109", 
		                        "/101", "/102", "/103" ,"/104", "/106", "/109", 
		                        ">101", ">102", ">103" ,">104", ">109", 
		                        "_101", "_102", "_103", "_104", "_109", 
		                        "*101", "*102", "*103", "*104", "*106", "*109", 
		                        "]101", "]102", "]103", "]104", "]106", "]109"};

	int64_t myChat = 0;

    const std::string photoFilePath = "_vectorP.png";
	const std::string photoMimeType = "image/png";

	VectorImage* editImage = nullptr;

	QSystemTrayIcon* trayIcon = nullptr;
	QDateTime fullTimeWork;

	QHash<int64_t, TcpClientForTelegram *>resultMassive;
	QHash<int64_t, VectorImage*>resultMassiveVector;
	QList<QString>chatIdMassive;

	int testCOunter = 0;

	QThread* longPollThread;
	LongPollWorker* longPollWorker;

	QTimer* destructionAndResurecctionTimer = nullptr;
};
