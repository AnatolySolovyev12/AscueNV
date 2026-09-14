#include<QApplication>
#include "TelegramJacket.h"
#include <qdatetime.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(1251);  // Установить UTF-8

	QApplication a(argc, argv);
	
	TelegramJacket * tgBotya = new TelegramJacket();

	return a.exec();
}