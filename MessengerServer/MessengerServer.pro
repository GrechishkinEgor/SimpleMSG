QT = core network sql

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../CommonClasses

SOURCES += \
        ../CommonClasses/chat.cpp \
        ../CommonClasses/generallog.cpp \
        ../CommonClasses/log.cpp \
        ../CommonClasses/message.cpp \
        ../CommonClasses/networkeventmainhandler.cpp \
        ../CommonClasses/requestid.cpp \
        ../CommonClasses/user.cpp \
        ../CommonClasses/userconnection.cpp \
        main.cpp \
        messengerdatabase.cpp \
        servernetwork.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../CommonClasses/chat.h \
    ../CommonClasses/generallog.h \
    ../CommonClasses/inetworkeventhandler.h \
    ../CommonClasses/log.h \
    ../CommonClasses/message.h \
    ../CommonClasses/networkeventmainhandler.h \
    ../CommonClasses/requestid.h \
    ../CommonClasses/user.h \
    ../CommonClasses/userconnection.h \
    messengerdatabase.h \
    servernetwork.h


