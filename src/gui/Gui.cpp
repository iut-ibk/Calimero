#include "Gui.h"
#include "../core/Language.h"

Gui::Gui(QWidget* parent, Qt::WFlags flags) : QMainWindow(parent, flags)
{
        model = NULL;
        translator = new QTranslator();
        QSettings settings;
        int language = settings.value("language",0).toInt();

        LanguageSelector::setLanguage(language, translator);


	setupUi(this);
}

Gui::~Gui()
{

}

void Gui::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        qDebug() << "Gui::changeEvent()";
        retranslateUi(this);
        break;
    default:
        break;
    }
}
