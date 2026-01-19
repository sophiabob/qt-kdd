#include "helpers.h"


void Helpers::timeDateMoscow(QString& result)
{
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.addSecs(3 * 3600);
    result = moscowDateTime.toString("yyyy-MM-dd HH:mm:ss"s);
}
