#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QVBoxLayout>
#include <QLineEdit> // Added for input field
#include <QLabel>    // Added for input field label
#include "spiralprogressindicator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слот для обработки завершения редактирования поля ввода
    void onInputEditingFinished();
    // Слот для обновления поля ввода при изменении значения прогресса
    void onProgressValueChanged(int value);

private:
    SpiralProgressIndicator *progressIndicator;
    QSlider *slider;
    QLineEdit *valueInput; // Поле ввода для значения
    QLabel *inputLabel;    // Метка для поля ввода
};

#endif // MAINWINDOW_H
