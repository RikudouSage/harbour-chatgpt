#ifndef LOCALEHELPER_H
#define LOCALEHELPER_H

#include <QObject>
#include <QLocale>

class LocaleHelper : public QObject
{
    Q_OBJECT
public:
    explicit LocaleHelper(QObject *parent = nullptr);
    Q_INVOKABLE QString languageName(const QString &localeName = QLocale::system().name()) const;

};

#endif // LOCALEHELPER_H
