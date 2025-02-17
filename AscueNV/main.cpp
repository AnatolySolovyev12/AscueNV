#include <QApplication>
#include "TelegramJacket.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	TelegramJacket tgBotya;

	return a.exec();
}