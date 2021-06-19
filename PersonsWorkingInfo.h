#pragma once

#include <QObject>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlError>
#include <QtQml>
#include <QSqlQuery>
#include <QSqlQueryModel>

class PersonsWorkingInfo : public QSqlQueryModel
{
public:
    PersonsWorkingInfo( QObject *parent = nullptr );
    QString GiveMeWorkingDataByNameAndDate( const QString &name, const QString &date );
    QString GiveMeWorkDataByNameAndDate( const QString &name, const QString &date );
    std::vector < std::string > GetTimeCheckVector();

private:
    int getMinutsFromWorkingData();
    std::vector< std::string > m_time_check_vector;

signals:
    void sendMessageToControllerToShowInQML( QString message );
};


