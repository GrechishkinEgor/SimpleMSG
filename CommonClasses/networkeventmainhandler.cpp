#include "networkeventmainhandler.h"
#include "generallog.h"

#include <QDateTime>

NetworkEventMainHandler::NetworkEventMainHandler()
{
    unrecognizedEventHandler = nullptr;
    nextMessageSize = 0;
}

void NetworkEventMainHandler::SetUnrecognizedEventHandler(INetworkEventHandler *handler)
{
    if (unrecognizedEventHandler)
        delete unrecognizedEventHandler;
    unrecognizedEventHandler = handler;
}

QMap<QString, INetworkEventHandler *> &NetworkEventMainHandler::GetHandlers()
{
    return handlers;
}

void NetworkEventMainHandler::SocketReadyRead()
{
    UserConnection* currentConnection = (UserConnection*)sender();
    QDataStream connectionInfo(currentConnection);
    connectionInfo.setVersion(QDataStream::Qt_6_2);
    if (connectionInfo.status() == QDataStream::Ok)
        //Считываем все сообщения из сокета, которые пришли полностью
        while(1)
        {
            //Пытаемся получить размер сообщения
            if (nextMessageSize == 0)
            {
                if (currentConnection->bytesAvailable() < sizeof(quint32))
                    break;
                connectionInfo >> nextMessageSize;
            }
            //Проверяем, что сообщение пришло полностью
            if (currentConnection->bytesAvailable() < nextMessageSize)
                break;

            //Читаем запрос клиента
            nextMessageSize = 0;            //Обнуляем переменную, чтобы принимать следующие сообщения
            QString message;                //Содержит суть запроса
            quint32 requestId;              //Идентификационный номер запроса
            connectionInfo >> message;
            connectionInfo >> requestId;
            currentConnection->GetConnectionStream() << message << requestId;   //Начинаем формировать ответ

            GeneralLog::GetLog()->Write(QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm:ss") +
                                        ". Socket connection " + QString::number(currentConnection->socketDescriptor()) + ". Message: " + message);

            //Определяем конкретный обработчик для запроса
            auto handler = handlers.find(message);
            if (handler == handlers.end())
            {
                GeneralLog::GetLog()->Write("Can't recognize message");
                unrecognizedEventHandler->Handle(requestId, currentConnection, connectionInfo);
            }
            else
            {
                GeneralLog::GetLog()->Write("Executing message " + message);
                (*handler)->Handle(requestId, currentConnection, connectionInfo);
            }

        }
}
