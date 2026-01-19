#include "helpers.h"


QString Helpers::timeDateMoscow()
{
    QDateTime utcDateTime = QDateTime::currentDateTimeUtc();
    QDateTime moscowDateTime = utcDateTime.addSecs(3 * 3600);
    return moscowDateTime.toString("yyyy-MM-dd HH:mm:ss");
}
