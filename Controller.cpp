#include "Controller.h"

Controller::Controller(QObject *parent)
    : QObject(parent)
{}

QString Controller::getWorkingMinutesByNameAndDate(const QString &name, const QString &date)
{
    QString result = m_Working_Info_Object.GiveMeWorkingDataByNameAndDate(name, date);
    qDebug()<<result;
    return result;
}

void Controller::showTimeCheckpoints()
{
    std::vector<std::string> current_info = m_Working_Info_Object.GetTimeCheckVector();
    for (auto item: current_info){
    QString temp = QString::fromUtf8(item.c_str());
    emit sendToQMLSignal(temp);
    }
}

