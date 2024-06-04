#include "userconnection.h"

User& UserConnection::GetUserInfo()
{
    return userInfo;
}

UserConnection::UserConnection()
{
    resultData = new QByteArray();
    resultStream = new QDataStream(resultData, QIODevice::WriteOnly);
    resultData->clear();
    resultStream->setVersion(QDataStream::Qt_6_2);
    savedSocketDescriptor = -1;

    (*resultStream) << quint32(0);
}

UserConnection::UserConnection(qintptr socketDescriptor)
{
    this->setSocketDescriptor(socketDescriptor);
    resultData = new QByteArray();
    resultStream = new QDataStream(resultData, QIODevice::WriteOnly);
    resultData->clear();
    resultStream->setVersion(QDataStream::Qt_6_2);
    savedSocketDescriptor = socketDescriptor;

    //Резервируем место, чтобы записать объем передаваемых данных
    (*resultStream) << quint32(0);
}

qintptr UserConnection::GetSavedSocketDescriptor()
{
    return savedSocketDescriptor;
}

QDataStream &UserConnection::GetConnectionStream()
{
    return *(this->resultStream);
}

void UserConnection::SendData()
{
    //Перед отправкой добавляем в начало 4х-байтовый размер пакета данных
    resultStream->device()->seek(0);
    (*resultStream) << quint32(resultData->size() - sizeof(quint32));
    this->write(*resultData);

    resultData->clear();
    resultStream->device()->seek(0);
    (*resultStream) << quint32(0);
}

void UserConnection::ClearConnectionStream()
{
    resultData->clear();
    resultStream->device()->seek(0);
    (*resultStream) << quint32(0);
}
