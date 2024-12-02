#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>


#include <tgbot/tgbot.h>

/*
class TelegramBot : public QObject {
    Q_OBJECT

public:
    TelegramBot(const QString& token, QObject* parent = nullptr)
        : QObject(parent), m_token(token) {
        m_manager = new QNetworkAccessManager(this);
        connect(m_manager, &QNetworkAccessManager::finished, this, &TelegramBot::onResult);
        getUpdates();

    }

private:
    QString m_token;
    QNetworkAccessManager* m_manager;


    QString URLQuery = "https://api.telegram.org/7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM/getMe";
    void getUpdates() {
        QUrl url(URLQuery);
        m_manager->get(QNetworkRequest(url));
    }

private slots:
    void onResult(QNetworkReply* reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
            QJsonArray resultArray = jsonDoc.object().value("result").toArray();

            for (const QJsonValue& value : resultArray) {
                QJsonObject obj = value.toObject();
                QString messageText = obj.value("message").toObject().value("text").toString();
                qDebug() << "Received message:" << messageText; // Выводим сообщение в qDebug()
            }
        }
        else {
            qDebug() << "Error:" << reply->errorString();
        }
        reply->deleteLater();
    }
};

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    QString token = "AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM"; // Замените на ваш токен
    TelegramBot bot(token);
    return a.exec();
}

#include "main.moc"
*/



#include <stdio.h>
#include <tgbot/tgbot.h>

int main() 
{
    TgBot::Bot bot("7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM");


    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) 
        {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
        });


    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        });



    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }



    return 0;
}