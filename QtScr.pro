SOURCES = main.cpp QtScr.cpp
HEADERS = QtScr.h
CONFIG += qt warn_on release thread
CONFIG += c++11
TRANSLATIONS = QtScr_es.ts \
               QtScr_ru.ts \
               QtScr_ua.ts
DEFAULTCODEC = ISO-8859-1
QT += widgets
QT += gui
