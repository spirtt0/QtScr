# QtScr

QtScr - small Qt Application for view ZX Spectrum's scr files. 
This program was forked and ported to Qt5

# To compile:

        qmake QtScr.pro
        make

# To "compile" translations:

        lrelease QtScr.pro      (qmake is a great tool, IMHO)

If you want add another translation to QtScr:

1. Edit QtScr.pro file
2. Add a line to TRANSLATIONS variable

        TRANSLATIONS = QtScr_es.ts \
                       QtScr_fr.ts

3. Run 'lupdate QtScr.pro'. This command create a QtScr_fr.ts file
4. Run the linguist tool, load the translation file and work.... :)
5. Run 'lrelease' QtScr.pro. This command create a QtScr_fr.qm file
6. Run QtScr and.... voila!
7. Send me the translated file QtScr_fr.ts to my e-mail address ;-)

# RUNNING QtScr

Pressing the right-button mouse, does appear a popup-menu. The options are
very easy to understand, I guess....
