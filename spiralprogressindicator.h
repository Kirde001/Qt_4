#ifndef SPIRALPROGRESSINDICATOR_H
#define SPIRALPROGRESSINDICATOR_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QPropertyAnimation> // Required for animation
#include <QEasingCurve>       // Required for animation easing

class SpiralProgressIndicator : public QWidget
{
    Q_OBJECT

    // Определяем progressValue как Q_PROPERTY.
    // WRITE будет функцией, которая ОБНОВЛЯЕТ внутреннее значение и перерисовывает виджет.
    Q_PROPERTY(int progressValue READ progressValue WRITE updateProgressValueAndRepaint NOTIFY progressValueChanged)
    Q_PROPERTY(int minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)
    Q_PROPERTY(int maximumValue READ maximumValue WRITE setMaximumValue NOTIFY maximumValueChanged)
    Q_PROPERTY(int wheelStepSize READ wheelStepSize WRITE setWheelStepSize NOTIFY wheelStepSizeChanged)

public:
    explicit SpiralProgressIndicator(QWidget *parent = nullptr);
    ~SpiralProgressIndicator() override = default; // Можно использовать default

    // Getters
    int progressValue() const;
    int minimumValue() const;
    int maximumValue() const;
    int wheelStepSize() const;

signals:
    void progressValueChanged(int value);
    void minimumValueChanged(int value);
    void maximumValueChanged(int value);
    void wheelStepSizeChanged(int size);

public slots:
    // Слот для ЗАПУСКА анимации к новому значению (вызывается извне)
    void setProgressValueAnimated(int value);

    // Сеттеры для min/max/step
    void setMinimumValue(int value);
    void setMaximumValue(int value);
    void setWheelStepSize(int size);

private slots:
    // Слот, вызываемый по завершении анимации
    void animationFinished();

    // Сеттер для Q_PROPERTY 'progressValue' (вызывается анимацией)
    void updateProgressValueAndRepaint(int value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    // Приватные члены для хранения значений
    int m_progressValue;
    int m_minimumValue;
    int m_maximumValue;
    int m_wheelStepSize;

    // Объект анимации
    QPropertyAnimation *m_animation;

    // Вспомогательная функция для ограничения значения
    int clampValue(int value) const;
};

#endif // SPIRALPROGRESSINDICATOR_H
