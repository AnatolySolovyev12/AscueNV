#include "MaxClass.h"

MaxClass::MaxClass(QObject* parent)
	: QObject(parent)
{
	AttachConsole(ATTACH_PARENT_PROCESS);
	getTokenFromFile();

	//connect(this, &MaxClass::sendIdNotificationForDelete, this, &MaxClass::deleteNotification);
	//connect(this, &MaxClass::sendUrlFile, this, &MaxClass::sendFileWithImage);

	QTimer::singleShot(1500, [this]() {
		manager = new QNetworkAccessManager(this);
		getStatusBoth(); }); // создаём его в рабочем потоке чтобы не было конфликтов разных потоков

	QTimer::singleShot(2000, [this]() { getLastMessageAsync(); });
}



void MaxClass::getStatusBoth()
{
	QUrl url(R"(https://platform-api2.max.ru/me)");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QByteArray authHeaderValue = tokenMaxBoth.toUtf8();
	request.setRawHeader("Authorization", authHeaderValue);

	QNetworkReply* reply = manager->get(request);

	QObject::connect(reply, &QNetworkReply::finished, [reply]() {
		if (reply->error() == QNetworkReply::NoError)
		{
			QByteArray response = reply->readAll();
			QJsonDocument responseDoc = QJsonDocument::fromJson(response);

			if (!responseDoc.isNull())
				qDebug() << "user_id: " << responseDoc["user_id"].toInt() << '\n' << "username: " << responseDoc["username"].toString() << '\n' << "name: " << responseDoc["name"].toString() << '\n';
			else
				qDebug() << '\n' << "responseDoc is NULL\n";
		}
		else
		{
			qDebug() << "Error code:" << reply->error();
			qDebug() << "Error text:" << reply->errorString();
			qDebug() << "Server response on error:" << reply->readAll();
		}
		reply->deleteLater();
		});
}



void MaxClass::sendMessage(QString chatId, const QString& message)
{
	QString urlString = QString("https://platform-api2.max.ru/messages?user_id=%1").arg(chatId);
	QUrl url(urlString);

	QJsonObject json;
	json["text"] = message; // Используем переданное сообщение

	// Преобразование JSON-объекта в строку
	QJsonDocument jsonDoc(json);
	QByteArray jsonData = jsonDoc.toJson();

	// Создание запроса
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QByteArray authHeaderValue = tokenMaxBoth.toUtf8();
	request.setRawHeader("Authorization", authHeaderValue);

	// Отправка запроса
	QNetworkReply* reply = manager->post(request, jsonData);

	// Обработчик ответа (если необходимо). Пригодится.
	QObject::connect(reply, &QNetworkReply::finished, [reply]() {

		if (reply->error() == QNetworkReply::NoError)
		{
			QString response = reply->readAll();
			qDebug() << '\n' << response << '\n';
		}
		else
			qDebug() << "Error:: " << reply->error();

		reply->deleteLater();
		});
}



void MaxClass::getTokenFromFile()
{
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
		qDebug() << "File is empty. Add a directory with a token (tokenMax.txt).";

		file.close();
		return;
	}

	file.close();

	tokenMaxBoth = myLine;

	qDebug() << "Token from file = " + tokenMaxBoth << '\n';

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

	QUrl url(R"(https://platform-api2.max.ru/updates)");

	QUrlQuery query;
	query.addQueryItem("limit", "30");  // Запрашиваем до 100 событий
	query.addQueryItem("timeout", "10"); // Сервер будет ждать события до 90 секунд
	url.setQuery(query);

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QByteArray authHeaderValue = tokenMaxBoth.toUtf8();
	request.setRawHeader("Authorization", authHeaderValue);

	// Отправка запроса
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
				QJsonArray messegeArray = responseDoc["updates"].toArray();

				if (messegeArray.isEmpty())
					std::cout << "\r" << QDate::currentDate().toString("dd.MM.yyyy").toStdString() << "   " << QTime::currentTime().toString().toStdString();
				else
				{
					qDebug() << "QJsonArray :\n" << messegeArray << "\n";

					for (auto val : messegeArray)
					{
						QJsonObject object = val.toObject();
						QJsonObject message = object["message"].toObject();
						QJsonObject body = message["body"].toObject();
						QString text = body["text"].toString();
						QJsonObject sender = message["sender"].toObject();
						QString userId = QString::number(sender["user_id"].toInteger());
						QString name = sender["name"].toString();

						qDebug() << "userId: " << userId << "\nname: " << name << "\nText: " << text << '\n';

						emit lastMessageReceived(qMakePair(userId, text));
					}
				}
			}

		}
		else
			qDebug() << "Error:" << reply->errorString();

		reply->deleteLater();
		isBusy = false;

		});
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

	if (!file.open(QIODevice::ReadOnly))
	{
		qWarning() << "Cannot open file:" << filePath << file.errorString();
		return;
	}

	const QByteArray fileData = file.readAll();
	file.close();

	QUrl url(R"(https://platform-api2.max.ru/uploads?type=image)");

	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QByteArray authHeaderValue = tokenMaxBoth.toUtf8();
	request.setRawHeader("Authorization", authHeaderValue);

	QNetworkReply* reply = manager->post(request, "");

	connect(reply, &QNetworkReply::finished, [this, reply, chatId, fileMessege, filePath]() {

		const QByteArray responseData = reply->readAll();

		if (reply->error() == QNetworkReply::NoError)
		{
			qDebug() << responseData;

			const QJsonDocument doc = QJsonDocument::fromJson(responseData);

			QJsonObject objURL = doc.object();

			if (!doc.isObject())
			{
				qWarning() << "uploadFile response is not JSON object";
				reply->deleteLater();
				return;
			}

			urlString = objURL.value("url").toString();

			if (urlString.isEmpty())
			{
				qWarning() << "urlFile is empty, response:" << responseData;
			}
			else
			{
				QUrl url(urlString);
				QNetworkRequest request(url);

				// 1. Создаем контейнер для multipart/form-data
				QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

				// 2. Создаем текстовую или файловую часть (часть формы)
				QHttpPart filePart;

				QVariant headerQVariant = QString("form-data; name=\"data\"; filename=\"%1\"").arg(fileMessege);

				filePart.setHeader(QNetworkRequest::ContentDispositionHeader, headerQVariant);

				// Указываем тип контента для файла (для картинок - image/png или image/jpeg)
				filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));

				// 3. Читаем сам файл с диска
				QFile* file = new QFile(filePath);

				if (!file->open(QIODevice::ReadOnly))
				{
					qWarning() << "Could not open file for reading!";
					delete multiPart;
					delete file;
					return;
				}

				// Привязываем устройство файла к части запроса
				filePart.setBodyDevice(file);

				// Делаем так, чтобы файл удалился из памяти автоматически вместе с multiPart
				file->setParent(multiPart);

				// Добавляем готовую часть с файлом в наш multipart-контейнер
				multiPart->append(filePart);

				// 4. Отправляем POST-запрос с бинарными данными формы вместо JSON строки
				QNetworkReply* reply = manager->post(request, multiPart);

				// Привязываем multiPart к reply, чтобы он удалился после завершения запроса
				multiPart->setParent(reply);

				// Обработчик ответа сервера загрузки
				QObject::connect(reply, &QNetworkReply::finished, [this, reply, chatId]() {

					if (reply->error() == QNetworkReply::NoError)
					{
						const QByteArray responseData = reply->readAll();
						qDebug() << "Upload Success! Server Response:" << responseData << '\n';

						const QJsonDocument doc = QJsonDocument::fromJson(responseData);

						QJsonObject objURL = doc.object();

						if (!doc.isObject())
						{
							qWarning() << "uploadFile response is not JSON object";
							reply->deleteLater();
							return;
						}

						QJsonObject photosObj = QJsonDocument::fromJson(responseData).object().value("photos").toObject();

						QJsonObject firstFileObj = photosObj.constBegin().value().toObject();

						QString urlString = QString("https://platform-api2.max.ru/messages?user_id=%1").arg(chatId);
						QUrl url(urlString);

						// 1. Создаем самый внутренний объект payload и добавляем туда токен
						QJsonObject payloadObj;
						payloadObj["token"] = firstFileObj.value("token").toString();;

						// 2. Создаем объект вложения, задаем тип и вкладываем туда наш payloadObj
						QJsonObject attachmentObj;
						attachmentObj["type"] = "image";
						attachmentObj["payload"] = payloadObj;

						// 3. Создаем массив attachments и добавляем туда объект вложения
						QJsonArray attachmentsArray;
						attachmentsArray.append(attachmentObj);

						// 4. Создаем корневой объект запроса
						QJsonObject rootObj;
						rootObj["text"] = "";
						rootObj["attachments"] = attachmentsArray;

						// 5. Преобразуем в документ и байтовый массив для отправки через QNetworkAccessManager
						QJsonDocument jsonDoc(rootObj);
						QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact); // Compact уберет лишние пробелы и переносы строк

						// Создание запроса
						QNetworkRequest request(url);
						request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

					
						QByteArray authHeaderValue = tokenMaxBoth.toUtf8();
						request.setRawHeader("Authorization", authHeaderValue);

						// Отправка запроса
						QNetworkReply* reply = manager->post(request, jsonData);

						// Обработчик ответа (если необходимо). Пригодится.
						QObject::connect(reply, &QNetworkReply::finished, [reply]() {

							if (reply->error() == QNetworkReply::NoError)
							{
								QString response = reply->readAll();
								qDebug() << response << '\n';
							}
							else
								qDebug() << "Error:: " << reply->error();

							reply->deleteLater();
							});
					}
					else
					{
						qDebug() << "Error:: " << reply->error();
						qDebug() << "Error text:: " << reply->errorString();
						qDebug() << "Server response on error:: " << reply->readAll();
					}
					reply->deleteLater();
					});
			}
		}
		else
		{
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