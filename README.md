# ColorPlot  
## Приложение переводит трехмерные данные в двухмерную картинку/The app converts three-dimensional data into a two-dimensional image  

### Как это работает?  
    На вход подается файл формата txt, который содержит таблицу с какими-либо данными.   
    Приложение нормирует эти данные в выбранном вами диапазоне цветов и выводит получившееся изображение.  
### Управление:  
    File->Add file - выбираем и загружаем необходимый файл.  
    Можно просто перетащить файл в окно приложения или воспользоваться вызовом через консоль.  
    Clear - очищает вывод.  
    Map->(black_gray_white,blue_white_red,green_yellow_red) - вывод будет состоять из этих трех цветов и их оттенков.  
    Color scaling - раскраска пересчивается только для данных, которые попали в область видимости пользователя.   
    Grid - добавляет сетку.  
    Изменение масштаба происходит с помощью колесика мышки. Можно зажать ПКМ и выделить необходимую область для отображения.  
    Если зажать ЛКМ, то можно передвигать изображение.  
    Если нажать на колесико мышки, то все действие над изображением отменятся.  

    #### Требования для сборки проекта:  
         Комплект: Qt5.14.2 MSVC2017/mingw73 32 или 64 bit.  

### Description  
    The input file is a txt format file that contains a table with some data.  
    The app normalizes this data in your selected range of colors and displays the resulting image.  
### How does it work?  
    File-> Add file - select and load file.  
    You can drag and drop the file into the application window or use CMD.  
    Clear - clears the output.  
    Map -> (black_gray_white, blue_white_red, green_yellow_red) - the output will consist of these three colors and their shades.  
    Color scaling - colors is calculated only for data that fell into the user's visibility area.  
    Grid - adds a grid.  
    Zooming in using the mouse wheel. You can hold down RMB and select the necessary area for display.
    If you hold LMB, you can move the image.  
    If you click on the mouse wheel, then all the action on the image is canceled.  

    #### Requirements for building the project:  
         Qt5.14.2 MSVC2017 / mingw73 32 or 64 bit.  

![](https://github.com/Diceorfl/ColorPlot/blob/master/ex.PNG?raw=true)  
![](https://github.com/Diceorfl/ColorPlot/blob/master/ex1.PNG?raw=true)  
![](https://github.com/Diceorfl/ColorPlot/blob/master/ex2.PNG?raw=true)  
![](https://github.com/Diceorfl/ColorPlot/blob/master/ex3.PNG?raw=true)  
![](https://github.com/Diceorfl/ColorPlot/blob/master/ex4.PNG?raw=true)  
![](https://github.com/Diceorfl/ColorPlot/blob/master/ex5.PNG?raw=true)  
![](https://github.com/Diceorfl/ColorPlot/blob/master/ex6.PNG?raw=true)  
![](https://github.com/Diceorfl/ColorPlot/blob/master/ex7.PNG?raw=true)  
