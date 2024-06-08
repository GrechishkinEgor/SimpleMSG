#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QTextStream>
#include <QFile>

class Log
{
private:
    QTextStream* logStream;
    QFile* logFile;
    QString logFileName;

    bool writingInConsole;
    bool writingInFile;
public:
    Log(QString logFileName);
    ~Log();

    /*Устанавливает, необходимо ли выводить логи в файл. По умолчанию вывод в файл выключен
     *flag = true, если да; flag = false, если нет
     *Возвращает истину, если переключение прошло успешно, или ложь, если переключение невозможно
    */
    bool WriteInFile(bool flag);
    /*Устанавливает, необходимо ли выводить логи в консоль. По умолчанию вывод в консоль включен
     *flag = true, если да; flag = false, если нет
     *Возвращает истину, если переключение прошло успешно, или ложь, если переключение невозможно
    */
    bool WriteInConsole(bool flag);

    /*Записывает строку логов*/
    void Write(QString msg);
};

#endif // LOG_H
