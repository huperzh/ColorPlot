/* Приложение строит цветовую схему высот трехмерного графика
 * Автор: Канониров Александр
 * Дата создания: 28.02.2020
 * Последнее изменение: 02.03.2020
 */
#include "clrplt.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    clrplt w;
    if(argc == 2)
        w.GetPath(argv[1]);
    w.show();

    return a.exec();
}
