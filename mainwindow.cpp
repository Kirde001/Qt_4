#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSlider>
#include "spiralprogressindicator.h"
#include <QWidget> // Убедимся, что QWidget включен для centralWidget

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Создаем экземпляр нашего виджета спирального индикатора
    progressIndicator = new SpiralProgressIndicator(this);

    // Создаем слайдер
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 100); // Начальный диапазон слайдера
    slider->setValue(0);      // Начальное значение слайдера

    // Создаем компоновщик для размещения виджетов
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(progressIndicator); // Добавляем индикатор
    layout->addWidget(slider);          // Добавляем слайдер

    // Создаем центральный виджет и устанавливаем для него компоновщик
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget); // Устанавливаем центральный виджет для главного окна

    // Соединяем сигналы и слоты для взаимодействия
    // При изменении значения слайдера, обновляем значение в индикаторе
    connect(slider, &QSlider::valueChanged, progressIndicator, &SpiralProgressIndicator::setProgressValue);

    // При изменении значения в индикаторе (в т.ч. от анимации или колесика), обновляем значение слайдера
    connect(progressIndicator, &SpiralProgressIndicator::progressValueChanged, slider, &QSlider::setValue);
    // При изменении минимального значения в индикаторе, обновляем диапазон слайдера
    connect(progressIndicator, &SpiralProgressIndicator::minimumValueChanged, slider, &QSlider::setMinimum);
    // При изменении максимального значения в индикаторе, обновляем диапазон слайдера
    connect(progressIndicator, &SpiralProgressIndicator::maximumValueChanged, slider, &QSlider::setMaximum);
}

MainWindow::~MainWindow()
{
    // Дочерние объекты (progressIndicator, slider, layout, centralWidget, animation)
    // будут удалены автоматически, так как у них установлен родитель (this)
}
