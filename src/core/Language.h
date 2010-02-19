#ifndef LANGUAGE_H
#define LANGUAGE_H

class QTranslator;

class LanguageSelector
{
public:
    static void setLanguage(int language, QTranslator *translator);
};

#endif // LANGUAGE_H
