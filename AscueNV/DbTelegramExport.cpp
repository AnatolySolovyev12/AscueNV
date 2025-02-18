#include "DbTelegramExport.h"

DbTelegramExport::DbTelegramExport(QObject* parent) : QObject(parent)
{
}

DbTelegramExport::~DbTelegramExport()
{
}

void DbTelegramExport::connectDataBase()
{
	mw_db = QSqlDatabase::addDatabase("QODBC"); // ��� ������� ODBC � Windows ���������� ������� ���������������� DNS � ����������������� �������. ������ �� ����� ��������.

	// mw_db.setHostName(myParamForSmtp->hostName); // ���� ��� ����� ��
	mw_db.setDatabaseName(odbcName); // ��������� ��� ����������������� DNS ������� ��� ������ � ������� �����.
	mw_db.setUserName("solexp");
	mw_db.setPassword("RootToor#");

	if (!mw_db.open()) // ��������� ��. ���� �� ��������� �� ����� false
	{
		mw_db.lastError().databaseText(); // ���� ���-�� ����� �� ��� �� ����� ��� � ����������
		mw_db.lastError().driverText();

		qDebug() << "Cannot open database: " << mw_db.lastError();
	}
}

void DbTelegramExport::queryDbResult(QString any)
{
	while (resultBool == false)
	{
		connectDataBase();

		QSqlQuery query;
		QString queryString;

		int iD = 0;
		int guidId;

		QDate curDate = QDate::currentDate();

		curDate = curDate.addDays(-1);

		QString timeInQuery = curDate.toString("yyyy-MM-dd"); // ������������� ������ ���� ��� ��� � ��.

		//queryString = "select ID_MeterInfo from MeterInfo where SN = '" + any + "'"; // ����������� ��������� ID �� ������ �������

		query.prepare("select ID_MeterInfo from MeterInfo where SN = :MeterInfoPrep"); // ���������� �������������� ������ � ������ ��� ������� �������� �� ��������
		query.bindValue(":MeterInfoPrep", any);

		//query.exec();
		//query.exec(queryString);
		//query.next();

		if (!query.exec() || !query.next())
		{
			qDebug() << "Query failed or no results: " << query.lastError();
			return;
		}

		iD = query.value(0).toInt();

		idFromDB = iD; // �������� �� ������� ������� iD

		if (!iD && odbcName == "DBEN")
		{
			mw_db.close();
			mw_db.removeDatabase(QSqlDatabase::defaultConnection);
			break;
		}

		if (!iD)
		{
			odbcName = "DBEN";
			mw_db.close();
			mw_db.removeDatabase(QSqlDatabase::defaultConnection);
			continue;
		}

		//queryString = "select TOP 1 ID_USPD from AutoInfo where info like '%" + any + "%'";
		queryString = QString("select TOP 1 ID_USPD from AutoInfo where info like '%%1%'").arg(any); // alternative
		query.exec(queryString);
		query.next();
		IdUSPD = query.value(0).toString();

		queryString = "select Name, TypeUSD, URL, NumUSD, PhoneNum from USD where ID_USPD = '" + IdUSPD + "'";
		query.exec(queryString);
		query.next();
		fullIp = query.value(0).toString() + " " + query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString() + " " + query.value(4).toString();

		ipForTcp = query.value(2).toString();

		queryString = "select ID_Point from MeterMountHist where ID_MeterInfo = '" + any.setNum(iD) + "'"; // �������� ID �� ��������
		//qDebug() << queryString;
		query.exec(queryString);
		query.next();
		iD = query.value(0).toInt();

		queryString = "select * from dbo.PointParams where ID_Point = '" + any.setNum(iD) + "' and ID_Param = '4'"; // �������� ID ��������� �������� ������� ��������
		//qDebug() << queryString;
		query.exec(queryString);
		query.next();

		iD = query.value(0).toInt();

		if (odbcName == "DBEN")
			queryString = "select Val, FORMAT(DT, 'yyyy-MM-dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '1' order by DT DESC";
		else
			queryString = "select Val, FORMAT(DT+1, 'yyyy-MM-dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '1' order by DT DESC";

		query.exec(queryString);
		query.next();
		day = query.value(0).toString();
		dateDay = query.value(1).toString();

		if (odbcName == "DBEN")
			queryString = "select Val, FORMAT(DT, 'yyyy-MM-dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '2' order by DT DESC";
		else
			queryString = "select Val, FORMAT(DT+1, 'yyyy-MM-dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '2' order by DT DESC";

		query.exec(queryString);
		query.next();
		night = query.value(0).toString();

		queryString = "select ID_Parent from NDIETable where ID_PP = '" + any.setNum(iD) + "'"; // �������� ID ��� ������������ ���������� GUID
		//qDebug() << queryString;
		query.exec(queryString);
		query.next();
		iD = query.value(0).toInt();

		queryString = "select Code from NDIETable where ID_DIE = '" + any.setNum(iD) + "'"; // �������� GUID
		//qDebug() << queryString;
		query.exec(queryString);
		query.next();
		guid = query.value(0).toString();

		resultBool = true;

		mw_db.close();
	}
	mw_db.removeDatabase(QSqlDatabase::defaultConnection);
	resultBool = false;
}

void DbTelegramExport::setAny(QString anyString)
{
	anyTelegramString = anyString;
}

QString DbTelegramExport::getAny()
{
	return anyTelegramString;
}

QString DbTelegramExport::getResult()
{
	if (!idFromDB) // �������� �� ������� ������� iD
		return "Not found device in DB";
	else
		return "\n" + dateDay + "\n" + "T1 = " + day + "  " + "T2 = " + night + "   " + "\n\n" + guid + "\n\n" + fullIp;
}

QString DbTelegramExport::getIpForTcp()
{
	return ipForTcp;
}
