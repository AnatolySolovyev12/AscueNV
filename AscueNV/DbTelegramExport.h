


#pragma once

//#include <QtWidgets/QMainWindow>


#include <QSqlDatabase>


#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtNetwork/QSslSocket>



class DbTelegramExport : QObject
{
	Q_OBJECT

public:
	DbTelegramExport(QObject* parent = nullptr);
	~DbTelegramExport();

	void connectDataBase();

	void queryDbResult(QString any);


	void setAny(QString anyString);

	QString getAny();


	QSqlDatabase mw_db;

private:

	QString day;
	QString night;
	QString guid;
	QString anyTelegramString;


};





