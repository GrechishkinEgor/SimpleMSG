#ifndef NETWORKEVENTMAINHANDLER_H
#define NETWORKEVENTMAINHANDLER_H

#include <QObject>
#include <QString>
#include <QMap>
#include "inetworkeventhandler.h"

class NetworkEventMainHandler : public QObject
{
    Q_OBJECT
protected:
    QMap<QString, INetworkEventHandler*> handlers;  //Конкретные обработчики событий сети
    INetworkEventHandler* unrecognizedEventHandler; //Обработчик для неизвестного события
    quint32 nextMessageSize;                        //Размер получаемого сообщения

public:
    NetworkEventMainHandler();
    void SetUnrecognizedEventHandler(INetworkEventHandler* handler);
    QMap<QString, INetworkEventHandler*>& GetHandlers();

public slots:
    void SocketReadyRead();                         //Прием запросов
};

#endif // NETWORKEVENTMAINHANDLER_H
