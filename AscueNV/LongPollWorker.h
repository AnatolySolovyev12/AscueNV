#pragma once

#include <QObject>
#include <QString>
#include <tgbot/tgbot.h>
#include <QTimer.h>

class LongPollWorker : public QObject
{
    Q_OBJECT
public:
    explicit LongPollWorker(TgBot::Bot* bot, QObject* parent = nullptr);
    ~LongPollWorker();

public slots:
    void doLongPoll();  // ���� ��� ������� blocking ������

signals:
    void messageReceived(const TgBot::Message::Ptr& message);
   // void errorOccurred(const QString& error);  // ������ ��� �������
    

    // ����� �������� ������� ��� �������� ������ � GUI

private:
    TgBot::Bot* bot_;
    TgBot::TgLongPoll* longPoll_;
    QTimer* myTimer;
};