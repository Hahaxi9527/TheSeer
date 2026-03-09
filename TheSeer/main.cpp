#include "lobby.h"
#include "start_game.h"
#include "theseer.h"
#include <QApplication>
manager mo;
int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    start_game w;
    w.show();
    return a.exec();
}
