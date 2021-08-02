#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QLocale>
#include <QDebug>
#include "QtScr.h"

/*
  The program starts here. It parses the command line and builds a message
  string to be displayed by the Hello widget.
*/

int main( int argc, char *argv[] )
{
    QApplication *app;
	QtScr *toplevel;

    app = new QApplication(argc, argv);

    QTranslator translator( 0 );
    translator.load(QLocale(), "QtScr", "_");
    app->installTranslator( &translator );

	toplevel = new QtScr();

    QObject::connect( toplevel, SIGNAL(clicked()), app, SLOT(quit()) );

    toplevel->show();
    return app->exec();
}
