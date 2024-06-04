QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

INCLUDEPATH += ../CommonClasses

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../CommonClasses/chat.cpp \
    ../CommonClasses/generallog.cpp \
    ../CommonClasses/log.cpp \
    ../CommonClasses/message.cpp \
    ../CommonClasses/networkeventmainhandler.cpp \
    ../CommonClasses/requestid.cpp \
    ../CommonClasses/user.cpp \
    ../CommonClasses/userconnection.cpp \
    applicationcontext.cpp \
    helloform.cpp \
    identificationform.cpp \
    main.cpp \
    messengermainform.cpp \
    registrationform.cpp \
    searchuserform.cpp

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
    applicationcontext.h \
    helloform.h \
    identificationform.h \
    message.h \
    messengermainform.h \
    registrationform.h \
    searchuserform.h

FORMS += \
    helloform.ui \
    identificationform.ui \
    messengermainform.ui \
    registrationform.ui \
    searchuserform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
