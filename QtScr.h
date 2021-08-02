#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>

#ifndef QTSCR_H
#define QTSCR_H

class QtScr : public QMainWindow
{
    Q_OBJECT
public:
    QtScr();
	~QtScr();
signals:
    void clicked(void);
protected:
    void mousePressEvent( QMouseEvent * );
    void paintEvent( QPaintEvent * );
    void dropEvent( QDropEvent * );
    void dragEnterEvent( QDragEnterEvent * );
private slots:
    void refreshScreen(void);
    void quit(void); 
	void AcercaDe(void);
	void CargaScr(void);
	void cambioBorder(void);
	void cambiaAtributos(void);
    void DobleTamano(void);

private:
	unsigned char Paper[256];
	unsigned char Ink[256];
	int ScrAddr[192];           // Direcciones de cada una de las líneas
    QString t;
	QImage *img;
	QPixmap *pm;
    QPixmap *doble;
    QTransform matriz;
	QTimer *timer;
	QTimer *timerFlash;
    QMenu *menu;
	int attrMenu;               // Identificador del menú de atributos On/Off
    int dobleMenu;              // Identificador del menú de ampliación
	bool noVerAttr;
    bool dobleTam;              // ¿Está a doble tamaño?
	bool  border;
	int flashOn;
	unsigned char *pScr;        // Buffer con la pantalla a visualizar
	unsigned char *pAttr;       // Buffer a los atributos

    bool leerFichero(const char *);
};

#endif
