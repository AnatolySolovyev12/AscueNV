#include "MaxClass.h"

MaxClass::MaxClass(QObject* parent)
	: QObject(parent)
{
	AttachConsole(ATTACH_PARENT_PROCESS);
	getTokenFromFile();

	connect(this, &MaxClass::sendIdNotificationForDelete, this, &MaxClass::deleteNotification);
	connect(this, &MaxClass::sendUrlFile, this, &MaxClass::sendFileWithImage);

	QTimer::singleShot(1500, [this]() { manager = new QNetworkAccessManager(this); }); // создаём его в рабочем потоке чтобы не было конфликтов разных потоков
	QTimer::singleShot(2000, [this]() { getLastMessageAsync(); });
}



void MaxClass::sendMessage(QString chatId, const QString& message)
{
	if (message.isEmpty()) {
		qWarning() << "Attempt to send empty message";
		return;
	}

	QString urlStringTemp = QString(R"(https://3100.api.green-api.com/waInstance%1/sendMessage/%2)")
		.arg(instanceNumber)
		.arg(tokenFromInstance);

	QUrl url(urlStringTemp);

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



void MaxClass::getTokenFromFile()
{
	//Берем за основу любой метод из API GreenAPI в котором фигурирует номер инстанса и токен

	QFile file(QCoreApplication::applicationDirPath() + "\\tokenMax.txt");

	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Don't find browse file. Add a directory with a token (tokenMax.txt).";
		return;
	}

	QTextStream out(&file);

	QString myLine = out.readLine(); // метод readLine() считывает одну строку из потока

	if (myLine == "")
	{
		qDebug() << "Don't find browse file. Add a directory with a token (tokenMax.txt).";

		file.close();
		return;
	}

	file.close();

	// Через регулярку получаем сначала номер инстанса

	QRegularExpression strPattern(QString(R"(waInstance([0-9]*))"));

	QRegularExpressionMatch matchReg = strPattern.match(myLine);

	if (matchReg.hasMatch())
	{
		instanceNumber = matchReg.captured().replace("waInstance", ""); // избавляемся от приставки заменяя её на пустоту
		qDebug() << "instanceNumber = " + instanceNumber;
	}
	else
		qDebug() << "No matches in RegEx for waInstance";

	// Полученный номер инстанса добавляем к паттерну и ищем совпадения для токена которым завершается любой метод из API

	strPattern.setPattern(QString(R"(waInstance%1/[\w]+/([\w]+))").arg(instanceNumber)); // группируем токен в отдельную группу в круглых скобках чтобы в дальнейшем извлечь через индекс

	matchReg = strPattern.match(myLine);

	if (matchReg.hasMatch())
	{
		tokenFromInstance = matchReg.captured(1); // извлекаем индексированную первую скобку в паттерне. (0) или () извлекает всё совпадение. (1) - то что было опоясано в паттерне круглыми скобками
		qDebug() << "tokenFromInstance = " + tokenFromInstance;
	}
	else
		qDebug() << "No matches in RegEx for tokenFromInstance";

	return;
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
	if (!manager) // защита от не инициализированно manager
	{
		QTimer::singleShot(100, this, &MaxClass::getLastMessageAsync);
		return;
	}

	if (isBusy) return;
	isBusy = true;

	QString urlStringTemp = QString(R"(https://3100.api.green-api.com/waInstance%1/receiveNotification/%2)")
		.arg(instanceNumber)
		.arg(tokenFromInstance);

	QUrl url(urlStringTemp);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QNetworkReply* reply = manager->get(request);

	emit startNetworkAccessSignal();

	QObject::connect(reply, &QNetworkReply::finished, [this, reply]() {

		emit finishNetworkAccessSignal();

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
					QString tempText = objText["textMessage"].toString();

					if (tempText.indexOf("**") >= 0 || tempText.indexOf("//") >= 0 || tempText.indexOf(">>") >= 0 || tempText.indexOf("__") >= 0) tempText = tempText.mid(1); // Fix проблемы с GreenAPI

					qDebug() << "Send messege...";
					emit lastMessageReceived(qMakePair(objId["chatId"].toString(), tempText));
				}
			}
			else
				std::cout << "\r" << QDate::currentDate().toString().toStdString() << "   " << QTime::currentTime().toString().toStdString();
		}
		else
			qDebug() << "Error:" << reply->errorString();

		reply->deleteLater();
		isBusy = false;

		});
}



void MaxClass::deleteNotification(QString idNotification)
{
	QString urlStringTemp = QString(R"(https://3100.api.green-api.com/waInstance%1/deleteNotification/%2/)")
		.arg(instanceNumber)
		.arg(tokenFromInstance);

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




void MaxClass::uploadFile(const QString& chatId, const QString& fileMessege, const QString& mime)
{
	const QString filePath = fileMessege;

	qDebug() << "filePath:" << filePath;

	QFile file(filePath);

	if (!file.exists())
	{
		qWarning() << "File does not exist:" << filePath;
		return;
	}
	else
		qDebug() << "File is Exist!";



	if (!file.open(QIODevice::ReadOnly))
	{
		qWarning() << "Cannot open file:" << filePath << file.errorString();
		return;
	}
	else
		qDebug() << "File is Open!";

	const QByteArray fileData = file.readAll();
	file.close();

	QString urlStringTemp = QString(R"(https://3100.api.green-api.com/waInstance%1/uploadFile/%2)")
		.arg(instanceNumber)
		.arg(tokenFromInstance);

	QUrl url(urlStringTemp);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "image/png");
	request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false); // без этого атрибута начинает ругаться на HTTP/2 почему то. Приходится работать с HTTP/1.1

	QNetworkReply* reply = manager->post(request, fileData);

	connect(reply, &QNetworkReply::finished, this, [this, chatId, fileMessege, reply]() {

		const QByteArray responseData = reply->readAll();

		if (reply->error() == QNetworkReply::NoError)
		{
			qDebug() << "uploadFile response:" << responseData;

			const QJsonDocument doc = QJsonDocument::fromJson(responseData);

			if (!doc.isObject())
			{
				qWarning() << "uploadFile response is not JSON object";
				reply->deleteLater();
				return;
			}
			else
				qDebug() << "File is JsonDocObject!";

			const QJsonObject obj = doc.object();
			const QString urlFile = obj.value("urlFile").toString();

			if (urlFile.isEmpty())
			{
				qWarning() << "urlFile is empty, response:" << responseData;
			}
			else
			{
				qDebug() << chatId << "   " << urlFile << "   " << fileMessege;
				emit sendFileWithImage(chatId, urlFile, fileMessege);
			}
		}
		else {
			qDebug() << "Upload error:" << reply->error() << reply->errorString();
			qDebug() << "Server reply:" << responseData;
		}

		reply->deleteLater();
		});
}



void MaxClass::sendFileWithImage(const QString& chatId, const QString& urlFile, const QString& fileName)
{
	if (urlFile.isEmpty()) {
		qWarning() << "Attempt to send empty message";
		return;
	}

	QString urlStringTemp = QString(R"(https://3100.api.green-api.com/waInstance%1/sendFileByUrl/%2)")
		.arg(instanceNumber)
		.arg(tokenFromInstance);

	QUrl url(urlStringTemp);

	QJsonObject json;
	json["chatId"] = chatId;
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