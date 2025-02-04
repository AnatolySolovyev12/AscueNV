#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

class TcpClientForTelegram : public QObject
{
    Q_OBJECT
public:
    explicit TcpClientForTelegram(QString any, QObject* parent = nullptr);

    ~TcpClientForTelegram();

    void connectToServer(const QString& host, quint16 port);
    void sendMessage(const QByteArray& message);
    void exchange();
    void summAnswer(QString& any);
    void summAnswervector(QString& any);
    QString returnResultString();
    void setResultString(QString any);
    void startToConnect(QString any);
    void resetAnswerString();

    void vecExchange();

signals:
    void messageReceived();
    void messageError();

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void exchangeFromTimer();

private:
    QTcpSocket* socket;

    bool connectedState = false;
    QTimer* myTimer;
    int counterForResend = 0;
    QString answerString;
    QString ip = "";
    int port = 8888;
    int reTransmitQuery = 0;
    QString serialStringForProtocol;
};

#endif // TCPCLIENT_H