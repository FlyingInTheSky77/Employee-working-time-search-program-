#pragma once

#include <QObject>
#include "PersonsWorkingInfo.h"

class Controller: public QObject
{
    Q_OBJECT

public:
    explicit Controller(QObject *parent=nullptr);
    Q_INVOKABLE QString getWorkingMinutesByNameAndDate(const QString &name, const QString &date);
    Q_INVOKABLE void showTimeCheckpoints();

signals:
    void sendToQMLSignal(QString message);
    void sendToQMLTimeCheckPointsSignal(QString message);

private:
    PersonsWorkingInfo m_Working_Info_Object;
};


