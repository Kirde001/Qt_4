#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QSlider;
class QLineEdit;
class QLabel;

#include "spiralprogressindicator.h" // Важно: Подключаем ваш виджет прогресса

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInputEditingFinished();
    void onProgressValueChanged(int value);

private:
    SpiralProgressIndicator *progressIndicator;
    QSlider *slider;
    QLabel *inputLabel;
    QLineEdit *valueInput;
};

#endif // MAINWINDOW_H
