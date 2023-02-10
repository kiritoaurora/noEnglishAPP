QT       += core gui
QT       += multimedia
QT       += core gui sql
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog.cpp \
    main.cpp \
    noenglish.cpp \
    widget.cpp

HEADERS += \
    dialog.h \
    noenglish.h \
    widget.h

FORMS += \
    dialog.ui \
    noenglish.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc \


DISTFILES += \
    ../icon/关闭 (1).png \
    ../icon/关闭.png \
    ../icon/向上箭头.png \
    ../icon/向下箭头.png \
    ../icon/密码登录.png \
    ../icon/搜索.png \
    ../icon/登录用户.png \
    ../icon/登录用户2.png \
    ../icon/键盘.png


