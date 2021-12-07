QT += core
QT -= gui

CONFIG += c++11

TARGET = recog
CONFIG += console
CONFIG -= app_bundle

QMAKE_CXXFLAGS+=-std=c++0x

TEMPLATE = app

SOURCES += main.cpp \
    utils/File.cpp \
    utils/boundary.cpp \
    utils/cutting.cpp \
    utils/choice.cpp \
    utils/choiceitem.cpp \
    utils/feature.cpp \
    utils/parseintputjson.cpp \
    items/settingcontent.cpp \
    items/supercontent.cpp \
    items/cardcontent.cpp \
    items/serialnumber.cpp \
    items/number.cpp \
    items/radio.cpp \
    items/checkbox.cpp \
    items/trueorfalse.cpp \
    items/subjectiveitem.cpp \
    items/absentflag.cpp \
    items/itemsblock.cpp \
    items/radiosimple.cpp \
    items/cardscore.cpp \
    items/pixelspercentscore.cpp \
    items/handwritingstudentno.cpp \
    items/handwritingcardscore.cpp \
    utils/imagemat.cpp \
    utils/util.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += utils/File.h \
    utils/boundary.h \
    utils/cutting.h \
    utils/choice.h \
    utils/choiceitem.h \
    utils/feature.h \
    utils/parseintputjson.h \
    items/settingcontent.h \
    items/supercontent.h \
    items/cardcontent.h \
    items/serialnumber.h \
    items/number.h \
    items/radio.h \
    items/checkbox.h \
    items/trueorfalse.h \
    items/subjectiveitem.h \
    items/absentflag.h \
    items/itemsblock.h \
    items/radiosimple.h \
    items/cardscore.h \
    items/pixelspercentscore.h \
    items/handwritingstudentno.h \
    items/handwritingcardscore.h \
    utils/imagemat.h \
    utils/util.h

INCLUDEPATH += /home/branches/clibs/jsoncpp/include \
             /home/branches/clibs/jsoncpp/src/lib_json \
             /usr/local/include \
            /usr/local/include/opencv4


LIBS += -L/usr/local/lib/ -llept -ltesseract \
        /home/branches/clibs/jsoncpp/build/lib/libjsoncpp.so \
        /usr/local/lib/libopencv_* \
        -lopencv_core -lopencv_imgcodecs -lopencv_highgui




