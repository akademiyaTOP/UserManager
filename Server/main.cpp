#include <QCoreApplication>
#include <QDebug>

int main (int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qDebug() << "Server is started";

    return app.exec();
}
