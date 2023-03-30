#include "localehelper.h"

LocaleHelper::LocaleHelper(QObject *parent) : QObject(parent)
{

}

QString LocaleHelper::languageName(const QString &localeName) const
{
    QLocale locale(localeName);
    return locale.nativeLanguageName();
}
