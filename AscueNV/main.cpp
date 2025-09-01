#include <QApplication>
#include "TelegramJacket.h"

QTimer* keepAliveTimer = new QTimer();

TelegramJacket * tgBotya = nullptr;

int main(int argc, char* argv[])
{
	QObject::connect(keepAliveTimer, &QTimer::timeout, []() { // watchdogs timer

		delete tgBotya;
		tgBotya = nullptr;
		tgBotya = new TelegramJacket();
		qDebug() << "Resurrection";
		});

	keepAliveTimer->setInterval(300000);

	QApplication a(argc, argv);

	keepAliveTimer->start();

	tgBotya = new TelegramJacket();

	QObject::connect(tgBotya, &TelegramJacket::signalForBreakResurrection, []() {
		keepAliveTimer->start();
		});

	return a.exec();
}