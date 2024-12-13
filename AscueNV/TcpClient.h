#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>


class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject* parent = nullptr);

    ~TcpClient();

    void connectToServer(const QString& host, quint16 port);
    void sendMessage(const QByteArray& message);
    void exchange();
    void summAnswer(QString& any);
    QString returnResultString();

    bool getReadyForAnswer();
    void setReadyForAnswer();
    void setUnReadyForAnswer();
    QString getAnswerString();
    void connectToServerWithGemor(QString any);

signals:
    void messageReceived(const QString& message);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);


private:
    QTcpSocket* socket;

    bool connectedState = false;
    QTimer* myTimer;
    int counterForResend = 0;
    QString answerString;
    QString ip = "";
    int port = 0;

    QString temporaryStringForIp;
    QString temporaryStringForPort;
    bool temporaryBool = false;

    bool readyForAnswer = false;
};

#endif // TCPCLIENT_H