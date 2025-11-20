#include <QApplication>
#include <QWidget>

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget w;
    w.setWindowTitle("Окно клиента");
    w.resize(400,200);
    w.show();

    return app.exec();
}
