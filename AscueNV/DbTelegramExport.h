#pragma once


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
	QString getResult();

private:

	QString day;
	QString night;
	QString guid;
	QString dateDay;
	QString IdUSPD;
	QString fullIp;
	QString anyTelegramString;
	QSqlDatabase mw_db;

	bool resultBool = false;
	QString odbcName = "DBEG";

};



