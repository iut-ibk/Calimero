#include "Language.h"
#include <QtCore>
#include <QTranslator>

void LanguageSelector::setLanguage(int language, QTranslator *translator)
{
    QSettings settings;
    QString path = settings.value("scriptpath","").toString() + "/language";
    QCoreApplication::instance()->removeTranslator(translator);

    switch(language)
    {
    case 0:
        if(!translator->load("calimero_de",path))
            qWarning() << "LanguageSelector::setLanguage() Error: Could not load language file";
        break;

    case 1:
        if(!translator->load("calimero_en",path))
            qWarning() << "LanguageSelector::setLanguage() Error: Could not load language file";
        break;


    default:
        if(!translator->load("calimero_de",path))
            qWarning() << "LanguageSelector::setLanguage() Error: Could not load language file";
        break;
    }

    QCoreApplication::instance()->installTranslator(translator);

}
