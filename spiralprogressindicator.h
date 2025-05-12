#ifndef SPIRALPROGRESSINDICATOR_H
#define SPIRALPROGRESSINDICATOR_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QPropertyAnimation> // Требуется для анимации свойств
#include <QEasingCurve>     // Требуется для кривых сглаживания анимации
#include <QDebug>           // Для отладочных сообщений

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
    // wheelStepSize - шаг изменения значения при вращении колесика мыши
    Q_PROPERTY(int wheelStepSize READ wheelStepSize WRITE setWheelStepSize NOTIFY wheelStepSizeChanged)

public:
    explicit SpiralProgressIndicator(QWidget *parent = nullptr);
    ~SpiralProgressIndicator(); // Деструктор для очистки ресурсов (хотя анимация имеет родителя)

    // Геттеры свойств
    int progressValue() const;
    int minimumValue() const;
    int maximumValue() const;
    int wheelStepSize() const; // Геттер для wheelStepSize

signals:
    // Сигналы, испускаемые при изменении свойств (для Q_PROPERTY NOTIFY)
    void progressValueChanged(int value);
    void minimumValueChanged(int value);
    void maximumValueChanged(int value);
    void wheelStepSizeChanged(int size);

public slots:
    // Сеттеры свойств (slots, так как могут быть вызваны из соединений или анимации)
    // setProgressValue - запускает анимацию изменения значения
    void setProgressValue(int value);
    void setMinimumValue(int value);
    void setMaximumValue(int value);
    void setWheelStepSize(int size); // Сеттер для wheelStepSize

private slots:
    // Слот, вызываемый по завершении анимации
    void animationFinished();

protected:
    // Переопределяем стандартные обработчики событий
    void paintEvent(QPaintEvent *event) override; // Для отрисовки виджета
    void wheelEvent(QWheelEvent *event) override; // Для обработки колесика мыши

private:
    // Приватные члены для хранения значений свойств
    int m_progressValue;
    int m_minimumValue;
    int m_maximumValue;
    int m_wheelStepSize; // Член для шага колесика

    // Объект анимации свойства progressValue
    QPropertyAnimation *m_animation;

    // Вспомогательная функция для ограничения значения в заданном диапазоне [min, max]
    int clampValue(int value) const;
};

#endif // SPIRALPROGRESSINDICATOR_H
