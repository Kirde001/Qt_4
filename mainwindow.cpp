#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSlider>
#include <QLineEdit> // Added for input field
#include <QLabel>    // Added for input field label
#include <QIntValidator> // Added for input validation
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

    // Создаем поле ввода и метку
    inputLabel = new QLabel("Значение:", this);
    valueInput = new QLineEdit(this);
    // Устанавливаем валидатор для поля ввода (только целые числа в диапазоне)
    valueInput->setValidator(new QIntValidator(progressIndicator->minimumValue(), progressIndicator->maximumValue(), this));
    valueInput->setText(QString::number(progressIndicator->progressValue())); // Устанавливаем начальное значение

    // Создаем компоновщик для размещения виджетов
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(progressIndicator); // Добавляем индикатор
    layout->addWidget(slider);            // Добавляем слайдер

    // Добавляем метку и поле ввода в горизонтальный компоновщик для аккуратного размещения
    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(valueInput);
    layout->addLayout(inputLayout); // Добавляем горизонтальный компоновщик в основной вертикальный

    // Создаем центральный виджет и устанавливаем для него компоновщик
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget); // Устанавливаем центральный виджет для главного окна

    // Соединяем сигналы и слоты для взаимодействия
    // При изменении значения слайдера, обновляем значение в индикаторе
    connect(slider, &QSlider::valueChanged, progressIndicator, &SpiralProgressIndicator::setProgressValue);

    // При изменении значения в индикаторе (в т.ч. от анимации, колесика, мыши), обновляем значение слайдера и поля ввода
    connect(progressIndicator, &SpiralProgressIndicator::progressValueChanged, slider, &QSlider::setValue);
    connect(progressIndicator, &SpiralProgressIndicator::progressValueChanged, this, &MainWindow::onProgressValueChanged);

    // При изменении минимального значения в индикаторе, обновляем диапазон слайдера и валидатор поля ввода
    connect(progressIndicator, &SpiralProgressIndicator::minimumValueChanged, slider, &QSlider::setMinimum);
    connect(progressIndicator, &SpiralProgressIndicator::minimumValueChanged, valueInput->validator(), &QValidator::setRange); // Обновляем диапазон валидатора

    // При изменении максимального значения в индикаторе, обновляем диапазон слайдера и валидатор поля ввода
    connect(progressIndicator, &SpiralProgressIndicator::maximumValueChanged, slider, &QSlider::setMaximum);
    connect(progressIndicator, &SpiralProgressIndicator::maximumValueChanged, valueInput->validator(), &QValidator::setRange); // Обновляем диапазон валидатора

    // При завершении редактирования поля ввода, обновляем значение индикатора
    connect(valueInput, &QLineEdit::editingFinished, this, &MainWindow::onInputEditingFinished);
}

MainWindow::~MainWindow()
{
    // Дочерние объекты будут удалены автоматически
}

void MainWindow::onInputEditingFinished()
{
    bool ok;
    int value = valueInput->text().toInt(&ok);
    // Проверяем, удалось ли преобразовать текст в число и находится ли оно в допустимом диапазоне
    if (ok) {
        progressIndicator->setProgressValue(value);
    } else {
        // Если ввод некорректен, можно вернуть предыдущее значение или показать ошибку
        // Пока просто возвращаем значение индикатора в поле ввода
        valueInput->setText(QString::number(progressIndicator->progressValue()));
    }
}

void MainWindow::onProgressValueChanged(int value)
{
    // Обновляем текст в поле ввода, временно блокируя сигналы, чтобы избежать зацикливания
    valueInput->blockSignals(true);
    valueInput->setText(QString::number(value));
    valueInput->blockSignals(false);
}
