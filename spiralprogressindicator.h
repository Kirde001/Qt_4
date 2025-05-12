#ifndef SPIRALPROGRESSINDICATOR_H
#define SPIRALPROGRESSINDICATOR_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent> // Added for mouse events
#include <QTimer>      // Added for timer-based animation
#include <QPointF>     // Added for mouse position tracking
#include <QtGlobal>    // Added for qBound

class SpiralProgressIndicator : public QWidget
{
    Q_OBJECT

    // Определяем свойства виджета
    // progressValue - текущее значение прогресса, анимируется
    Q_PROPERTY(int progressValue READ progressValue WRITE setProgressValue NOTIFY progressValueChanged)
    // minimumValue - минимально допустимое значение
    Q_PROPERTY(int minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)
    // maximumValue - максимально допустимое значение
    Q_PROPERTY(int maximumValue READ maximumValue WRITE setMaximumValue NOTIFY maximumValueChanged)

public:
    explicit SpiralProgressIndicator(QWidget *parent = nullptr);
    ~SpiralProgressIndicator();

    // Геттеры свойств
    int progressValue() const;
    int minimumValue() const;
    int maximumValue() const;

signals:
    // Сигналы, испускаемые при изменении свойств (для Q_PROPERTY NOTIFY)
    void progressValueChanged(int value);
    void minimumValueChanged(int value);
    void maximumValueChanged(int value);

public slots:
    // Сеттеры свойств (slots, так как могут быть вызваны из соединений или анимации)
    // setProgressValue - запускает анимацию изменения значения
    void setProgressValue(int value);
    void setMinimumValue(int value);
    void setMaximumValue(int value);

private slots:
    // Слот для анимации прогресса
    void animateProgress();

protected:
    // Переопределяем стандартные обработчики событий
    void paintEvent(QPaintEvent *event) override; // Для отрисовки виджета
    void wheelEvent(QWheelEvent *event) override; // Для обработки колесика мыши
    void mousePressEvent(QMouseEvent *event) override; // Для начала перетаскивания мышью
    void mouseMoveEvent(QMouseEvent *event) override; // Для обработки движения мыши при перетаскивании
    void mouseReleaseEvent(QMouseEvent *event) override; // Для завершения перетаскивания мышью

private:
    // Приватные члены для хранения значений свойств
    int m_progressValue;
    int m_minimumValue;
    int m_maximumValue;
    int m_targetValue; // Целевое значение для анимации

    // Таймер для анимации
    QTimer m_animationTimer;

    // Члены для обработки перетаскивания мышью
    bool m_isDragging;
    QPointF m_lastMousePos;
    int m_initialProgressValue;
};

#endif // SPIRALPROGRESSINDICATOR_H
