#-------------------------------------------------
#
# Project created by QtCreator 2019-07-16T16:21:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AdminModule_QT
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17 c++14
DEFINES += NOMINMAX
# LIBS += -User32 -Advapi32

SOURCES += \
        ../../Core/UnitAuth.cpp \
        ../../Core/UnitClOptions.cpp \
        ../../Core/UnitCommon.cpp \
        ../../Core/UnitComputers.cpp \
        ../../Core/UnitEncode.cpp \
        ../../Core/UnitFines.cpp \
        ../../Core/UnitGames.cpp \
        ../../Core/UnitIDList.cpp \
        ../../Core/UnitList.cpp \
        ../../Core/UnitLog.cpp \
        ../../Core/UnitLogRecords.cpp \
        ../../Core/UnitOptions.cpp \
        ../../Core/UnitPassword.cpp \
        ../../Core/UnitRandCounter.cpp \
        ../../Core/UnitSLList.cpp \
        ../../Core/UnitSend.cpp \
        ../../Core/UnitShellState.cpp \
        ../../Core/UnitStates.cpp \
        ../../Core/UnitSync.cpp \
        ../../Core/UnitSyncMsgs.cpp \
        ../../Core/UnitTariffs.cpp \
        ../../Core/UnitTimeRand.cpp \
        ../../Core/UnitUsers.cpp \
        ../../Ext/BrianGladman/SHA/hmac.c \
        ../../Ext/BrianGladman/SHA/sha1.c \
        ../../Ext/BrianGladman/SHA/sha2.c \
        ../../Ext/ZilongTan/fast-hash/fasthash.c \
        dialogAuth.cpp \
        dialogComputers.cpp \
        dialogFine.cpp \
        dialogFines.cpp \
        dialogLogIn.cpp \
        dialogNewPass.cpp \
        dialogOpenConfig.cpp \
        dialogOptions.cpp \
        dialogOptionsPass.cpp \
        dialogTariffTimes.cpp \
        dialogTariffs.cpp \
        dialogUserPass.cpp \
        dialogUsers.cpp \
        main.cpp \
        mainWindow.cpp \
        unitGlobal.cpp

HEADERS += \
        ../../Core/UnitAuth.h \
        ../../Core/UnitClOptions.h \
        ../../Core/UnitCommon.h \
        ../../Core/UnitComputers.h \
        ../../Core/UnitEncode.h \
        ../../Core/UnitFines.h \
        ../../Core/UnitGames.h \
        ../../Core/UnitIDList.h \
        ../../Core/UnitList.h \
        ../../Core/UnitLog.h \
        ../../Core/UnitLogRecords.h \
        ../../Core/UnitOptions.h \
        ../../Core/UnitPassword.h \
        ../../Core/UnitRandCounter.h \
        ../../Core/UnitSLList.h \
        ../../Core/UnitSend.h \
        ../../Core/UnitShellState.h \
        ../../Core/UnitStates.h \
        ../../Core/UnitSync.h \
        ../../Core/UnitSyncMsgs.h \
        ../../Core/UnitTariffs.h \
        ../../Core/UnitTimeRand.h \
        ../../Core/UnitUsers.h \
        ../../Ext/BrianGladman/SHA/hmac.h \
        ../../Ext/BrianGladman/SHA/sha1.h \
        ../../Ext/BrianGladman/SHA/sha2.h \
        ../../Ext/ZilongTan/fast-hash/fasthash.h \
        dialogAuth.h \
        dialogComputers.h \
        dialogFine.h \
        dialogFines.h \
        dialogLogIn.h \
        dialogNewPass.h \
        dialogOpenConfig.h \
        dialogOptions.h \
        dialogOptionsPass.h \
        dialogTariffTimes.h \
        dialogTariffs.h \
        dialogUserPass.h \
        dialogUsers.h \
        mainWindow.h \
        unitGlobal.h

FORMS += \
        dialogAuth.ui \
        dialogComputers.ui \
        dialogFine.ui \
        dialogFines.ui \
        dialogLogIn.ui \
        dialogNewPass.ui \
        dialogOpenConfig.ui \
        dialogOptions.ui \
        dialogOptionsPass.ui \
        dialogTariffTimes.ui \
        dialogTariffs.ui \
        dialogUserPass.ui \
        dialogUsers.ui \
        mainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

LIBS += -lUSER32 -lADVAPI32 -lWS2_32 -lNTDLL -lBCRYPT -lIPHLPAPI -lUSERENV

RESOURCES += \
    resources.qrc
