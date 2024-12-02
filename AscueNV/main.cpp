#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>

class TelegramBot : public QObject {
    Q_OBJECT

public:
    TelegramBot(QObject* parent = nullptr)
        : QObject(parent)
    {
        m_manager = new QNetworkAccessManager(this);
        connect(m_manager, &QNetworkAccessManager::finished, this, &TelegramBot::onResult);
        getUpdates();
    }

private:
    QString m_token = "7880555988:AAHhHkQUARdmJXUT8RB7mrXIgVTQIAkN3RM"; // Замените на ваш токен
    QNetworkAccessManager* m_manager;

    void getUpdates() {
        QUrl url(QString("https://api.telegram.org/bot%1/getUpdates").arg(m_token));
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
       // reply->deleteLater();
    }
};

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    
    TelegramBot bot();
    return a.exec();
}