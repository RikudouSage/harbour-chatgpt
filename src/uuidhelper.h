#ifndef UUIDHELPER_H
#define UUIDHELPER_H

#include <QString>
#include <QUuid>

inline QString uuidToString(QUuid uuid) {
    return uuid.toString().replace("{", "").replace("}", "");
}

#endif // UUIDHELPER_H
