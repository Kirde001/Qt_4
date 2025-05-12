#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSlider>
#include "spiralprogressindicator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Use the new class name
    progressIndicator = new SpiralProgressIndicator(this);

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 100);
    slider->setValue(0);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(progressIndicator);
    layout->addWidget(slider);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
    connect(slider, &QSlider::valueChanged, progressIndicator, &SpiralProgressIndicator::setProgressValue);

    connect(progressIndicator, &SpiralProgressIndicator::progressValueChanged, slider, &QSlider::setValue);
    connect(progressIndicator, &SpiralProgressIndicator::minimumValueChanged, slider, &QSlider::setMinimum);
    connect(progressIndicator, &SpiralProgressIndicator::maximumValueChanged, slider, &QSlider::setMaximum);

}

MainWindow::~MainWindow()
{
}
