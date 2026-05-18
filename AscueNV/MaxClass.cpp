#include "MaxClass.h"

MaxClass::MaxClass(QObject* parent)
	: QObject(parent), manager(new QNetworkAccessManager)
{
	AttachConsole(ATTACH_PARENT_PROCESS);
	urlString = QString(getTokenFromFile());
	//chatId = getChatIdFromFile();

	connect(this, &MaxClass::sendIdNotificationForDelete, this, &MaxClass::deleteNotification);
	connect(this, &MaxClass::sendUrlFile, this, &MaxClass::sendFileWithImage);


	QTimer::singleShot(2000, [this]() { getLastMessageAsync(); });
}



void MaxClass::sendMessage(QString chatId, QString message)
{
	if (message.isEmpty()) {
		qWarning() << "Attempt to send empty message";
		return;
	}

	QUrl url(urlString);

	QJsonObject json;
	json["chatId"] = chatId;
	json["message"] = message; // Используем переданное сообщение

	// Преобразование JSON-объекта в строку
	QJsonDocument jsonDoc(json);
	QByteArray jsonData = jsonDoc.toJson();

	// Создание запроса
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	// Отправка запроса
	QNetworkReply* reply = manager->post(request, jsonData);

	// Обработчик ответа (если необходимо). Пригодится.
	QObject::connect(reply, &QNetworkReply::finished, [reply]() {

		if (reply->error() == QNetworkReply::NoError)
		{
			QString response = reply->readAll();
			qDebug() << response;
		}
		else
		{
			qDebug() << "Error:: " << reply->error();
		}
		reply->deleteLater();
		});
}



QString MaxClass::getTokenFromFile()
{
	QFile file(QCoreApplication::applicationDirPath() + "\\tokenMax.txt");

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Don't find browse file. Add a directory with a token (tokenMax.txt).";
		return 0;
	}

	QTextStream out(&file);

	QString myLine = out.readLine(); // метод readLine() считывает одну строку из потока

	if (myLine == "")
	{
		qDebug() << "Don't find browse file. Add a directory with a token (tokenMax.txt).";
		file.close();
		return 0;
	}

	file.close();

	return myLine;
}



QString MaxClass::getChatIdFromFile()
{
	QFile file(QCoreApplication::applicationDirPath() + "\\chatIdMax.txt");

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Don't find browse file. Add a directory with a token (chatIdMax.txt).";
		return 0;
	}

	QTextStream out(&file);

	QString myLine = out.readLine(); // метод readLine() считывает одну строку из потока

	if (myLine == "")
	{
		qDebug() << "Don't find browse file. Add a directory with a token (chatIdMax.txt).";
		file.close();
		return 0;
	}

	file.close();

	return myLine;
}



void MaxClass::getLastMessageAsync()
{
	if (isBusy) return;
	isBusy = true;
	QString urlStringTemp = QString("https://3100.api.green-api.com/waInstance3100514553/receiveNotification/134edc19c6c64e4f971e4578b787f54725492643c588466095");

	QUrl url(urlStringTemp);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = manager->get(request);

	QObject::connect(reply, &QNetworkReply::finished, [this, reply]() {
		
		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray responseData = reply->readAll();
			QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);

			if (!responseDoc.isNull())
			{
				qDebug() << "\nreceiptId: " << responseDoc["receiptId"].toInt();

				QJsonObject objBody = responseDoc["body"].toObject();
				QJsonObject objMessage = objBody["messageData"].toObject();
				QJsonObject objText = objMessage["textMessageData"].toObject();

				qDebug() << "text: " << objText["textMessage"].toString();

				QJsonObject objId = objBody["senderData"].toObject();

				qDebug() << "chatId: " << objId["chatId"].toString();

				emit sendIdNotificationForDelete(QString::number(responseDoc["receiptId"].toInt()));

				if (objBody["typeWebhook"].toString() != "outgoingAPIMessageReceived")
				{
					qDebug() << "Send messege...";
					emit lastMessageReceived(qMakePair<QString, QString>(objId["chatId"].toString(), objText["textMessage"].toString()));
				}
			}
			else
				std::cout << "\r" << QDate::currentDate().toString().toStdString() << "   " << QTime::currentTime().toString().toStdString();
		}
		else
			qDebug() << "Error:" << reply->errorString();

		reply->deleteLater();
		isBusy = false;

		// Самовызов через 5 секунд
		QTimer::singleShot(3000, this, &MaxClass::getLastMessageAsync);
		});
}



void MaxClass::deleteNotification(QString idNotification)
{
	QString urlStringTemp = QString("https://3100.api.green-api.com/waInstance3100514553/deleteNotification/134edc19c6c64e4f971e4578b787f54725492643c588466095/");

	urlStringTemp += idNotification;

	QUrl url(urlStringTemp);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = manager->deleteResource(request);

	QObject::connect(reply, &QNetworkReply::finished, [this, reply]() {

		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray responseData = reply->readAll();
			QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);

			if (!responseDoc.isNull())
			{
				if (!responseDoc["result"].toBool())
					qDebug() << responseDoc["reason"].toString();
				else
					qDebug() << "Notification was delete";
			}
			else
				qDebug() << "Not array or null";
		}
		else
			qDebug() << "Error:" << reply->errorString();

		reply->deleteLater();
		}
	);
}




void MaxClass::uploadFile(QString chatId, QString message, QString mime)
{
	const QString filePath = QCoreApplication::applicationDirPath() + "/" + message;

	QFile file(filePath);
	if (!file.exists()) {
		qWarning() << "File does not exist:" << filePath;
		return;
	}
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning() << "Cannot open file:" << filePath << file.errorString();
		return;
	}

	QUrl url("https://3100.api.green-api.com/waInstance3100514553/uploadFile/134edc19c6c64e4f971e4578b787f54725492643c588466095");

	QNetworkRequest request(url);
	request.setRawHeader("Content-Type", "image/png");

	QNetworkReply* reply = manager->post(request, &file);

	connect(reply, &QNetworkReply::finished, this, [this, chatId, message, reply]() {

		if (reply->error() == QNetworkReply::NoError) 
		{
			QByteArray responseData = reply->readAll();
			QJsonDocument responseDoc = QJsonDocument::fromJson(responseData);

			if (!responseDoc.isNull())
			{
				qDebug() << "\nurlFile: " << responseDoc["urlFile"].toInt();

				emit sendUrlFile(chatId, responseDoc["urlFile"].toString(), message);
			}
			else
				std::cout << "Json for uploadFile is NULL array";
		}
		else 
		{
			qDebug() << "Upload error:" << reply->error() << reply->errorString();
			qDebug() << "Server reply:" << reply->readAll();
		}

		reply->deleteLater();
		});
}




void MaxClass::sendFileWithImage(QString chatId, QString urlFile, QString fileName)
{
	if (urlFile.isEmpty()) {
		qWarning() << "Attempt to send empty message";
		return;
	}

	QUrl url(urlString);

	QJsonObject json;
	json["urlFile"] = urlFile;
	json["fileName"] = fileName; // Используем переданное сообщение

	// Преобразование JSON-объекта в строку
	QJsonDocument jsonDoc(json);
	QByteArray jsonData = jsonDoc.toJson();

	// Создание запроса
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	// Отправка запроса
	QNetworkReply* reply = manager->post(request, jsonData);

	// Обработчик ответа (если необходимо). Пригодится.
	QObject::connect(reply, &QNetworkReply::finished, [reply]() {

		if (reply->error() == QNetworkReply::NoError)
		{
			QString response = reply->readAll();
			qDebug() << response;
		}
		else
		{
			qDebug() << "Error:: " << reply->error();
		}
		reply->deleteLater();
		});
}