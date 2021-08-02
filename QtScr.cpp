#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QImage>
#include <QPainter>
#include <QMimeData>
#include <QFile>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <stdlib.h>

#include "QtScr.h"

QtScr::QtScr()
{
	// Contrucción de las tablas de Paper/Ink
	for( int idx = 0; idx < 256; idx++ )
	{
		Ink[idx]   = idx & 0x07;
		Paper[idx] = (idx >> 3) & 0x07;
		if( idx & 0x40 )  // Hay Bright???
		{
			Ink[idx]   |= 0x08;
			Paper[idx] |= 0x08;
		}
		Ink[idx] += 32;
		Paper[idx] += 32;
	}

	// Construcción de la tabla de direcciones de la pantalla
	for( int linea = 0; linea < 24; linea++ )
	{
		int idx, lsb, msb, addr;
		lsb = ((linea & 0x07) << 5);
		msb = linea & 0x18;
		addr = msb * 256 + lsb;
		idx = linea * 8;
		for( int scan = 0; scan < 8; scan++, addr += 256 )
			ScrAddr[scan + idx] = addr;
	}
	   
	pScr = new unsigned char[6144];
	pAttr = new unsigned char[768];
	for( int idx = 0; idx < 768; idx++ )
		pAttr[idx] = 0;

	flashOn = 0; // inicializamos el ciclo de flash
    border = true;  // color blanco por defecto para el borde

	pm = new QPixmap;
    doble = new QPixmap;
    matriz.scale((double)2.0, (double)2.0);

    img = new QImage(256, 192, QImage::Format_Indexed8);
    img->setColorCount(64);
	img->setColor(32, qRgb(0, 0, 0));        /* Black */
	img->setColor(33, qRgb(0, 0, 192));      /* Blue */
	img->setColor(34, qRgb(192, 0, 0));      /* Red */
	img->setColor(35, qRgb(192, 0, 192));    /* Magenta */
	img->setColor(36, qRgb(0, 192, 0));      /* Green */
	img->setColor(37, qRgb(0, 192, 192));    /* Cyan */
	img->setColor(38, qRgb(192, 192, 0));    /* Yellow */
	img->setColor(39, qRgb(192, 192, 192));  /* White */
	img->setColor(40, qRgb(0, 0, 0));        /* Black */
	img->setColor(41, qRgb(0, 0, 255));      /* Bright Blue */
	img->setColor(42, qRgb(255, 0, 0));      /* Bright Red */
	img->setColor(43, qRgb(255, 0, 255));    /* Bright Magenta */
	img->setColor(44, qRgb(0, 255, 0));      /* Bright Green */
	img->setColor(45, qRgb(0, 255, 255));    /* Bright Cyan */
	img->setColor(46, qRgb(255, 255, 0));    /* Bright Yellow */
	img->setColor(47, qRgb(255, 255, 255));  /* Bright White */

    timer = new QTimer(this);
    connect( timer, SIGNAL(timeout()), SLOT(refreshScreen()) );

	// Crear el menú Popup
    menu = new QMenu("QtScr Menu",this);
    ////Q_CHECK_PTR( menu );
    menu->addAction(tr("Load screen$..."), this, SLOT(CargaScr()));
    menu->addAction(tr("Change border color"), this, SLOT(cambioBorder()));

    QAction* attrMenu = menu->addAction(tr("Screen without ATTR$"), this,
                                    SLOT(cambiaAtributos()), Qt::Key_Space);
    attrMenu->setCheckable(true);

    QAction* dobleMenu = menu->addAction(tr("&Double size"), this, SLOT(DobleTamano()),
                                     Qt::Key_D);
    dobleMenu->setCheckable(true);

    menu->addSeparator();
    menu->addAction(tr("About..."), this, SLOT(AcercaDe()));
    menu->addSeparator();
    menu->addAction(tr("Exit"), this, SLOT(quit()));

    noVerAttr = false;   // por defecto se ven los atributos

	// Redimensionar la ventana al tamaño del Spectrum (256x192)
	// más un borde por cada lado de 32 pixels de ancho
    resize( 320, 256 );

	// Inicializar el color del fondo
    QPalette palette;
    palette.setColor(backgroundRole(), Qt::lightGray);
    setPalette(palette);

	// Activar funcionalidad de Drag & Drop
    setAcceptDrops(true);

    dobleTam = false;
}

QtScr::~QtScr()
{
	delete []pScr;
	delete []pAttr;
	delete pm;
	delete img;
}

/*
 * Cambiar el cliclo de flash cada 16/50 de segundo (aka 320 ms)
 */
void QtScr::refreshScreen(void)
{
  flashOn = !flashOn;
  repaint();
}

/*
 * Gestiona las pulsaciones de los botones del ratón
 */
void QtScr::mousePressEvent( QMouseEvent *event )
{
	// Si se ha pulsado el botón central del ratón, mostrar el menú
	if( event->button() == Qt::RightButton )
		menu->popup(event->globalPos());
}

/*
 * Rutina de dibujo de la pantalla
 */
void QtScr::paintEvent( QPaintEvent * )
{
	unsigned int paper, ink;
	int addr;
	unsigned char val, flash, attr, *pixel;
	register unsigned char bit;
	register unsigned char mask;

	for( int cordy = 0, nAttr = 0; cordy < 192; cordy++ )
	{
		addr = ScrAddr[cordy];
        nAttr = (cordy & 0xF8) << 2;
		pixel = img->scanLine(cordy);
		for( int columna = 0; columna < 32; columna++ )
		{
			if( noVerAttr )  // pintar la pantalla sin atributos
			{
				ink = 32;
				paper = 39;
				flash = 0;
			}
			else             // pintar la pantalla en colorines
			{
				attr = pAttr[nAttr++];
				ink = Ink[attr];
				paper = Paper[attr];
				flash = (attr & 0x80) && flashOn;
			}
			val = pScr[addr++];

			for( mask = 0x80; mask; mask >>= 1, pixel++ )
			{
				bit = ((val & mask) ? 1 : 0) ^ flash;
				if( bit )
					*pixel = ink;
				else
					*pixel = paper;
			}
		}
	}

	pm->convertFromImage(*img);
    if( dobleTam )
    {
        *doble = pm->transformed( matriz );
        QPainter p(this);
        p.drawPixmap(64,64,*doble);
    }
    else {
        QPainter p(this);
        p.drawPixmap(32,32,*pm);
    }
}

/*
 * Se ocupa de la funcionalidad del Drag & Drop
 */
void QtScr::dropEvent( QDropEvent *dndzone )
{
    QStringList list;
	QString last;

    last = dndzone->mimeData()->text();

    if ( last.lastIndexOf(".scr", -1, Qt::CaseInsensitive) != -1)
	{
		timer->stop();
        last.remove(0, 5);
        leerFichero(last.toUtf8().constData());
		if( noVerAttr )
			cambiaAtributos();
		else
		{
            repaint();
			timer->start( 320 );
            int pos = last.lastIndexOf('/', -1, Qt::CaseInsensitive);
			QString titulo = "QtScr: ";
            titulo += last.rightRef( last.length() - ++pos );
            setWindowTitle(titulo);
		}

	}
	else
		QMessageBox::information(0, tr("Warning!"),
                                 tr("This file isn't a Spectrum screen dump"));
}

void QtScr::dragEnterEvent( QDragEnterEvent *qdragenterevent )
{
    if (qdragenterevent->mimeData()->hasText()) qdragenterevent->acceptProposedAction();
}

bool QtScr::leerFichero(const char *filename)
{

	QFile fp(filename);

	int nRead;

    if( (fp.open(QIODevice::ReadOnly)) == false )
	{
		QMessageBox::warning(0, tr("Warning!"), tr("Can't open file"));
        return false;
	}

    nRead = fp.read((char *)pScr, 6144);
    if( nRead != 6144 )
    {
        printf("Warning: Se han leído %d datos de pantalla\n", nRead);
        printf("         desde el fichero \"%s\"\n", filename);
    }

    nRead += fp.read((char *)pAttr, 768);
	if( nRead != 6912 )
	{
        printf("Warning: Se han leído %d datos de atributos\n", nRead);
        printf("         desde el fichero \"%s\"\n", filename);
        QMessageBox::warning(0, tr("Warning!"),
                             tr("Error loading screen$"));
	}

	fp.close();

    return true;
}

void QtScr::CargaScr(void)
{
	QString filename;

	timer->stop();

    filename = QFileDialog::getOpenFileName(this,tr("Open screen$ dump"),0,
                                            tr("Images(*.scr)"));
	if( !filename.isNull() )
	{
        leerFichero(filename.toUtf8().constData());
        int pos = filename.lastIndexOf('/', -1,Qt::CaseInsensitive);
        QString titulo = "QtScr: ";

		if( pos != -1 )
            titulo += filename.rightRef( filename.length() - ++pos );
		else
			titulo += filename.data();

        setWindowTitle(titulo);
	}
	else
		return;

	if( noVerAttr )
		cambiaAtributos();
	else
	{
        repaint();
		timer->start( 320 );
	}
}

void QtScr::cambioBorder(void)
{
	timer->stop();

	// Cambiar el color del fondo
    border = !border;
    if( border ) {
        QPalette palette;
        palette.setColor(backgroundRole(), Qt::lightGray);
        setPalette(palette);
        }
    else {
        QPalette palette;
        palette.setColor(backgroundRole(), Qt::black);
        setPalette(palette);
    }

    repaint();

	timer->start( 320 );
}

void QtScr::AcercaDe(void)
{
		QMessageBox::about(this, tr("About QtScr"),
                           tr("\nQtScr v0.10\n"
                           "Spectrum screen$ viewer\n"
                           "(c) 1999-2003 José Luis Sánchez Villanueva\n"
                           "jsanchez@todounix.homeip.net\n\n"
                           "Port to Qt5\n"
                           "(c) 2021 Alexandr Bolsvet\n"
                           "spirtt0@gmail.com\n"));
}

void QtScr::cambiaAtributos(void)
{
	if( noVerAttr )
	{
        noVerAttr = false;
		timer->start(320);
	}
	else
	{
        noVerAttr = true;
		timer->stop();
	}
    repaint();
}

void QtScr::DobleTamano(void)
{
    if( dobleTam )
    {
        dobleTam = false;
        resize(320, 256);
    }
    else
    {
        dobleTam = true;
        resize(640, 512); 
    }
}

void QtScr::quit(void)
{
	exit(0);
}
