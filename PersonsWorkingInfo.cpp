#include "PersonsWorkingInfo.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QtQml>
#include <QSqlQuery>

static void connectToDatabase()
{
    QSqlDatabase database = QSqlDatabase::database();
    if ( !database.isValid() )
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        if ( !database.isValid() )
            qFatal( "Cannot add database: %s", qPrintable( database.lastError().text() ) );
    }
    const QDir writeDir = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
    if ( !writeDir.mkpath(".") )
        qFatal( "Failed to create writable directory at %s", qPrintable( writeDir.absolutePath() ) );
    // Ensure that we have a writable location on all devices.
    const QString fileName = writeDir.absolutePath() + "/Working_Database";
    // When using the SQLite driver, open() will create the SQLite database if it doesn't exist.
    database.setDatabaseName( fileName );
    if ( !database.open() )
    {
        qFatal( "Cannot open database: %s", qPrintable( database.lastError().text() ) );
        QFile::remove( fileName );
    }
}

static void createTableArchive()
{
    //create this table for testing program
    if ( QSqlDatabase::database().tables().contains( QStringLiteral( "archive" ) ) )
        return;    
    QSqlQuery query;
    if ( !query.exec(
        "CREATE TABLE IF NOT EXISTS 'archive' ("
        "   '_id' INT NOT NULL,"
        "   'datetime' DATETIME NOT NULL,"
        "   'card_id' INT NOT NULL,"
        "FOREIGN KEY(card_id) REFERENCES card_id(_id)"
        "   PRIMARY KEY (_id)"
        ")")) {
        qFatal( "Failed to query database: %s", qPrintable( query.lastError().text() ) );
    }
    //TEXT as ISO8601 strings ("YYYY-MM-DD HH:MM:SS.SSS").
    query.exec( "INSERT INTO archive VALUES(1,'2021-04-09 08:55:10', 2)" );
    query.exec( "INSERT INTO archive VALUES(2,'2021-04-09 13:01:10', 2)" );
    query.exec( "INSERT INTO archive VALUES(3,'2021-04-09 09:01:10', 1)" );
    query.exec( "INSERT INTO archive VALUES(4,'2021-04-09 18:01:10', 2)" );
    query.exec( "INSERT INTO archive VALUES(5,'2021-04-09 17:59:10', 1)" );
    query.exec( "INSERT INTO archive VALUES(6,'2021-04-08 09:00:10', 1)" );
    query.exec( "INSERT INTO archive VALUES(7,'2021-04-08 17:59:10', 1)" );
    query.exec( "INSERT INTO archive VALUES(8,'2021-04-01 09:00:10', 3)" );
    query.exec( "INSERT INTO archive VALUES(9,'2021-04-01 17:59:10', 3)" );
    query.exec( "INSERT INTO archive VALUES(10,'2021-04-09 09:00:10', 4)" );
    query.exec( "INSERT INTO archive VALUES(11,'2021-04-09 13:00:00', 4)" );
    query.exec( "INSERT INTO archive VALUES(12,'2021-04-09 14:00:00', 4)" );
    query.exec( "INSERT INTO archive VALUES(13,'2021-04-09 18:05:10', 4)" );
}

static void createTableCard()
{
    //create this table for testing program
    if ( QSqlDatabase::database().tables().contains( QStringLiteral( "card_id" ) ) )
        return;
    QSqlQuery query;
    if ( !query.exec(
        "CREATE TABLE IF NOT EXISTS 'card_id' ("
        "   '_id' INT NOT NULL,"
        "   'info' TEXT NOT NULL,"
        "   PRIMARY KEY(_id)"
        ")")) {
        qFatal( "Failed to query database: %s", qPrintable( query.lastError().text() ) );
    }
    query.exec( "INSERT INTO card_id VALUES(1,'Rodin')" );
    query.exec( "INSERT INTO card_id VALUES(2,'Petrov')" );
    query.exec( "INSERT INTO card_id VALUES(3,'Ivanov')" );
    query.exec( "INSERT INTO card_id VALUES(4,'Sidorov')" );
}

//TEXT as ISO8601 strings ("YYYY-MM-DD HH:MM:SS.SSS").

PersonsWorkingInfo::PersonsWorkingInfo ( QObject *parent )
    : QSqlQueryModel( parent )
{
    connectToDatabase();
    createTableArchive();
    createTableCard();
}

QString PersonsWorkingInfo::GiveMeWorkingDataByNameAndDate( const QString &name, const QString &date )
{
    if ( QSqlDatabase::database().tables().contains( QStringLiteral( "card_id" ) ) )
        return;
    QSqlQuery query;
    query.prepare( "SELECT archive.datetime, card_id.info, card_id._id, archive.card_id FROM card_id, "
                  "archive WHERE card_id._id = archive.card_id AND card_id.info = :name" );
    query.bindValue( ":name", name );
    query.exec();
    m_time_check_vector.clear();
    QVector < QString > time_check_QVector;
    while ( query.next() )
    {
        QString test = query.value( 1 ).toString();
        std::string test_in_std = test.toUtf8().constData();
        if ( name == query.value(1).toString() )
        {
            QString time_check = query.value( 0 ).toString();
            std::string time_in_std = time_check.toUtf8().constData();
            std::string data = time_in_std.substr( 0, 10 );
            if ( data == date.toUtf8().constData() )
            {
                std::string temp = time_in_std.substr( 11, 18 );
                m_time_check_vector.push_back( temp );//TODO in one line
            }
        }
     }
     setQuery( query );
     if ( lastError().isValid() )
        return "There are problems with the database or communication with it";

     if ( m_time_check_vector.size() == 0)
        return QStringLiteral("Employee %1 didn't work on %3. \n It is "
                                  "recommended to check the spelling of the employee's surname"
                                  " or check if the date is entered correctly").arg(name).arg(date);

     if ( ( m_time_check_vector.size() % 2 ) != 0 )
               return "not enough timestamps for correct timing. "
                      "Perhaps the employee forgot to check in, "
                      "when he came in or went to office with colleagues."
                      "Click \"show all time records\"";
     
     int working_time_in_minuts = getMinutsFromWorkingData();
     QString message_with_working_min = QStringLiteral( "Employee %1 worked for %2 minutes "
                                                              "on %3." ).arg( name ).arg( working_time_in_minuts ).arg( date );
     return message_with_working_min;
     }
}

int PersonsWorkingInfo::getMinutsFromWorkingData()
{
    enum class check_description{ in, out };
    check_description check_mark = check_description::in;
    int working_time_in_minuts = 0;
    for ( const auto& time_check: m_time_check_vector )
    {
        std::string hour_string = time_check.substr( 0, 2 );
        std::string min_string = time_check.substr( 3, 2 );
        std::string sec_string = time_check.substr( 6, 2 );
        int hour = stoi( hour_string );
        int min = stoi( min_string );
        int sec = stoi( sec_string );
        if ( sec >= 30)
            min++;
        int total_min = hour * 60 + min;
        if ( check_mark == check_description::in )
        {
            working_time_in_minuts -= total_min;
            check_mark = check_description::out;
        }
        else
        {
            working_time_in_minuts += total_min;
            check_mark = check_description::in;
        }
    }
    return working_time_in_minuts;
}
