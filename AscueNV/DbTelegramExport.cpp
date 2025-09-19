#include "DbTelegramExport.h"

DbTelegramExport::DbTelegramExport(QObject* parent) : QObject(parent)
{
}

DbTelegramExport::~DbTelegramExport()
{
}

void DbTelegramExport::connectDataBase()
{
	mw_db = QSqlDatabase::addDatabase("QODBC"); // Для раблоты ODBC в Windows необходимо задвать пользовательский DNS в администрировании системы. Иначен не будет работать.

	// mw_db.setHostName(myParamForSmtp->hostName); // хост где лежит БД
	mw_db.setDatabaseName(odbcName); // указываем имя пользовательского DNS который был создан в системе ранее.
	mw_db.setUserName("solexp");
	mw_db.setPassword("RootToor#");

	if (!mw_db.open()) // открываем БД. Если не открывает то вернёт false
	{
		mw_db.lastError().databaseText(); // если что-то пойдёт не так то пишем это в переменные
		mw_db.lastError().driverText();

		qDebug() << "Cannot open database: " + mw_db.lastError().text();
	}
}

void DbTelegramExport::queryDbResult(QString any)
{
	while (resultBool == false)
	{
		connectDataBase();

		QSqlQuery query;
		QString queryString = "%" + any + "%";

		int iD = 0;
		int guidId;

		QDate curDate = QDate::currentDate();

		curDate = curDate.addDays(-1);

		QString timeInQuery = curDate.toString("yyyy-MM-dd"); // Разворачиваем формат даты так как в БД.

		query.prepare("select ID_Point from Points where PointName like :IdPrep"); // используем подготовленный запрос в начале как хорошую практику от инъекций. : - обязательный спецсимвол для указания вставки заполнителя.
		query.bindValue(":IdPrep", queryString); // связываем вставку заполнитель со с переменной

		if (!query.exec() || !query.next())
		{
			qDebug() << "Not found ID_Point for this device in DB: " + query.lastError().text();
		}

		iD = query.value(0).toInt();

		idFromDB = iD; // проверка на возврат пустого iD

		if (!iD && odbcName == "DBEN")
		{
			qDebug() << "Not found device. Correct your query.\n";
			mw_db.close();
			mw_db.removeDatabase(QSqlDatabase::defaultConnection);
			break;
		}

		if (!iD)
		{
			qDebug() << "Not found device in " + odbcName + " dataBase. Trying in DBEN dataBase.\n";
			odbcName = "DBEN";
			mw_db.close();
			mw_db.removeDatabase(QSqlDatabase::defaultConnection);
			continue;
		}
		
		queryString = "%" + any + "%";
		query.prepare("select TOP (1) ID_USPD from AutoInfo where info like :IdPrep order by ID_USPD"); // используем подготовленный запрос в начале как хорошую практику от инъекций. : - обязательный спецсимвол для указания вставки заполнителя.
		                                                                                                // + обязательно с TOP делать OREDR BY т.к. ему так легче искать
		query.bindValue(":IdPrep", queryString); // связываем вставку заполнитель со с переменной
		
		if (!query.exec() || !query.next())
		{
			qDebug() << "Not found ID_USPD for this ID_Point" + QString::number(iD) + " in DB: " + query.lastError().text();
		}

		IdUSPD = query.value(0).toString();

		queryString = "select Name, TypeUSD, URL, NumUSD, PhoneNum from USD where ID_USPD = '" + IdUSPD + "'"; // получаем ряд целевой информации
		query.exec(queryString);
		query.next();
		fullIp = query.value(0).toString() + " " + query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString() + " " + query.value(4).toString();

		ipForTcp = query.value(2).toString();

		queryString = "select * from dbo.PointParams where ID_Point = '" + any.setNum(iD) + "' and ID_Param = '4'"; // получаем ID параметра активной энергии счётчика
		query.exec(queryString);
		query.next();

		iD = query.value(0).toInt();

		if (odbcName == "DBEN") // А+ Т1
			queryString = "select Val, FORMAT(DT, 'yyyy-MM-dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '1' order by DT DESC";
		else
			queryString = "select Val, FORMAT(DT+1, 'yyyy-MM-dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '1' order by DT DESC";

		query.exec(queryString);
		query.next();
		day = query.value(0).toString();
		dateDay = query.value(1).toString();

		if (odbcName == "DBEN") // А+ Т2
			queryString = "select Val, FORMAT(DT, 'yyyy-MM-dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '2' order by DT DESC";
		else
			queryString = "select Val, FORMAT(DT+1, 'yyyy-MM-dd') as DT from dbo.PointRatedNIs where  ID_PP = '" + any.setNum(iD) + "' and N_Rate = '2' order by DT DESC";

		query.exec(queryString);
		query.next();
		night = query.value(0).toString();

		queryString = "select ID_Parent from NDIETable where ID_PP = '" + any.setNum(iD) + "'"; // получаем ID для последующего получаения GUID
		//qDebug() << queryString;
		query.exec(queryString);
		query.next();
		iD = query.value(0).toInt();

		queryString = "select Code from NDIETable where ID_DIE = '" + any.setNum(iD) + "'"; // получаем GUID
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
	if (!idFromDB) // проверка на возврат пустого iD
		return "Not found device in DB";
	else
		return "\n" + dateDay + "\n" + "T1 = " + day + "  " + "T2 = " + night + "   " + "\n\n" + guid + "\n\n" + fullIp;
}

QString DbTelegramExport::getIpForTcp()
{
	return ipForTcp;
}
