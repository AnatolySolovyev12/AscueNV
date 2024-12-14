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
    explicit TcpClientForTelegram(QObject* parent = nullptr);

    ~TcpClientForTelegram();

    void connectToServer(const QString& host, quint16 port);
    void sendMessage(const QByteArray& message);
    void exchange();
    void summAnswer(QString& any);
    QString returnResultString();
    void startToConnect(QString any);

signals:
    void messageReceived(const QString message);

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
    int port = 8888;
};

#endif // TCPCLIENT_H