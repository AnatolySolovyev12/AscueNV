#include <QApplication>
#include "TelegramJacket.h"
#include <qdatetime.h>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	TelegramJacket * tgBotya = new TelegramJacket();

	return a.exec();
}