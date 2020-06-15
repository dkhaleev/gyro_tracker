QT       += \
        core \
        gui \
        opengl \
        serialport \
        widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += 	c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET =        GyroScope

TEMPLATE =      app

SOURCES += \
        src/generalTab.cpp \
        src/main.cpp \
        src/mainwindow.cpp \
        src/rOc_serial.cpp \
        src/rOc_timer.cpp 

HEADERS += \
        include/generalTab.h \
        include/mainwindow.h \
        include/rOc_serial.h \
        include/rOc_timer.h

FORMS += \
        ui/mainwindow.ui
	
INCLUDEPATH += \
	src \
	include \
	ui
	
OBJECTS_DIR = tmp/
MOC_DIR = tmp/
DESTDIR = bin/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

static { # everything below takes effect with CONFIG ''= static
 CONFIG+= static
 CONFIG += staticlib # this is needed if you create a static library, not a static executable
 DEFINES+= STATIC
 message("~~~ static build ~~~") # this is for information, that the static build is done
 mac: TARGET = $$join(TARGET,,,_static) #this adds an _static in the end, so you can seperate static build
}

# change the nama of the binary, if it is build in debug mode
CONFIG (debug, debug|release) {
 mac: TARGET = $$join(TARGET,,,_debug)
 win32: TARGET = $$join(TARGET,,,d)
}
