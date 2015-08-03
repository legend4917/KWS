#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T22:33:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KWS
TEMPLATE = app


SOURCES += main.cpp\
        kws.cpp \
    myaudio.cpp \
    edit.cpp

HEADERS  += \
    kws.h \
    myaudio.h \
    edit.h

FORMS    += kws.ui

QT += multimedia

DISTFILES += \
    Cluster.exe \
    HBuild.exe \
    HCompV.exe \
    HCopy.exe \
    HDMan.exe \
    HERest.exe \
    HHEd.exe \
    HInit.exe \
    HLEd.exe \
    HList.exe \
    HLMCopy.exe \
    HLRescore.exe \
    HLStats.exe \
    HMMIRest.exe \
    HParse.exe \
    HQuant.exe \
    HRest.exe \
    HResults.exe \
    HSGen.exe \
    HSLab.exe \
    HSmooth.exe \
    HVite.exe \
    LAdapt.exe \
    LBuild.exe \
    LFoF.exe \
    LGCopy.exe \
    LGList.exe \
    LGPrep.exe \
    LLink.exe \
    LMerge.exe \
    LNewMap.exe \
    LNorm.exe \
    LPlex.exe \
    LSubset.exe

RESOURCES += \
    qss.qrc

RC_FILE = Project.rc
