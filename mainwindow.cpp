#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QIntValidator>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Создаем экземпляр нашего виджета спирального индикатора
    progressIndicator = new SpiralProgressIndicator(this);

    // Создаем слайдер
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 100); // Начальный диапазон слайдера
    slider->setValue(0);      // Начальное значение слайдера

    // Создаем поле ввода и метку
    inputLabel = new QLabel("Значение:", this);
    valueInput = new QLineEdit(this);

    // Устанавливаем валидатор для поля ввода (только целые числа в диапазоне)
    auto *validator = new QIntValidator(progressIndicator->minimumValue(), progressIndicator->maximumValue(), this);
    valueInput->setValidator(validator);
    valueInput->setText(QString::number(progressIndicator->progressValue())); // Устанавливаем начальное значение

    // Создаем компоновщик для размещения виджетов
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(progressIndicator); // Добавляем индикатор
    layout->addWidget(slider);            // Добавляем слайдер

    // Добавляем метку и поле ввода в горизонтальный компоновщик
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(valueInput);
    layout->addLayout(inputLayout);

    // Создаем центральный виджет и устанавливаем компоновку
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Соединяем сигналы и слоты
    connect(slider, &QSlider::valueChanged, progressIndicator, &SpiralProgressIndicator::setProgressValue);
    connect(progressIndicator, &SpiralProgressIndicator::progressValueChanged, slider, &QSlider::setValue);
    connect(progressIndicator, &SpiralProgressIndicator::progressValueChanged, this, &MainWindow::onProgressValueChanged);
    connect(valueInput, &QLineEdit::editingFinished, this, &MainWindow::onInputEditingFinished);

    // Обновление валидатора при изменении диапазонов
    connect(progressIndicator, &SpiralProgressIndicator::minimumValueChanged, this, [=](int min) {
        slider->setMinimum(min);
        validator->setBottom(min);
    });
    connect(progressIndicator, &SpiralProgressIndicator::maximumValueChanged, this, [=](int max) {
        slider->setMaximum(max);
        validator->setTop(max);
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::onInputEditingFinished()
{
    bool ok = false;
    int value = valueInput->text().toInt(&ok);
    if (ok) {
        progressIndicator->setProgressValue(value);
    }
}

void MainWindow::onProgressValueChanged(int value)
{
    valueInput->setText(QString::number(value));
}
