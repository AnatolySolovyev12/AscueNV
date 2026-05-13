#pragma once

#include <qapplication.h>
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QUrlQuery>
#include <windows.h>
#include <QFile.h>
#include <iostream>


class MaxClass : public QObject
{
	Q_OBJECT

public:
	MaxClass(QObject* parent = nullptr);
	QString getTokenFromFile();
	QString getChatIdFromFile();
	void getLastMessageAsync();
	void deleteNotification(QString idNotification);

signals:
	void lastMessageReceived(const QPair<QString, QString>takePair);
	void sendIdNotificationForDelete(QString id);

public slots:
	void sendMessage(QString chatId, QString message);

private:
	QNetworkAccessManager* manager = nullptr;
	QString chatId = "";
	QString urlString = "";
	bool isBusy = false;
};