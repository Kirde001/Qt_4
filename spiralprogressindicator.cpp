#include "spiralprogressindicator.h"
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>
#include <QtMath> // Для qDegreesToRadians, qCos, qSin
#include <QTimer>
#include <QDebug>

SpiralProgressIndicator::SpiralProgressIndicator(QWidget *parent)
    : QWidget(parent), m_progressValue(0), m_maximumValue(100), m_minimumValue(0), m_targetValue(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(&m_animationTimer, &QTimer::timeout, this, &SpiralProgressIndicator::animateProgress);
    m_animationTimer.start(16); // ~60 FPS
}

void SpiralProgressIndicator::setProgressValue(int value)
{
    m_targetValue = qBound(m_minimumValue, value, m_maximumValue);
}

int SpiralProgressIndicator::progressValue() const
{
    return m_progressValue;
}

void SpiralProgressIndicator::setMaximumValue(int value)
{
    m_maximumValue = value;
    // Ensure targetValue is within new bounds
    m_targetValue = qBound(m_minimumValue, m_targetValue, m_maximumValue);
    // If current value is outside, animate towards target
    if (m_progressValue < m_minimumValue || m_progressValue > m_maximumValue) {
         m_progressValue = qBound(m_minimumValue, m_progressValue, m_maximumValue);
         update(); // Update immediately if current value was outside bounds
         // Start animation if needed
         if (m_progressValue != m_targetValue && !m_animationTimer.isActive()) {
             m_animationTimer.start(16);
         }
    } else {
       update(); // Update to reflect potential change in max value influencing rendering
    }
}

int SpiralProgressIndicator::maximumValue() const
{
    return m_maximumValue;
}

void SpiralProgressIndicator::setMinimumValue(int value)
{
    m_minimumValue = value;
     // Ensure targetValue is within new bounds
    m_targetValue = qBound(m_minimumValue, m_targetValue, m_maximumValue);
    // If current value is outside, animate towards target
     if (m_progressValue < m_minimumValue || m_progressValue > m_maximumValue) {
         m_progressValue = qBound(m_minimumValue, m_progressValue, m_maximumValue);
         update(); // Update immediately if current value was outside bounds
         // Start animation if needed
         if (m_progressValue != m_targetValue && !m_animationTimer.isActive()) {
             m_animationTimer.start(16);
         }
    } else {
       update(); // Update to reflect potential change in min value influencing rendering
    }
}

int SpiralProgressIndicator::minimumValue() const
{
    return m_minimumValue;
}

void SpiralProgressIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());
    QRectF outerRect(0, 0, side, side);
    outerRect.moveCenter(rect().center());

    double progressRatio = static_cast<double>(m_progressValue - m_minimumValue) / (m_maximumValue - m_minimumValue);
    // Handle division by zero if min == max
    if (m_maximumValue == m_minimumValue) {
        progressRatio = 0.0;
    }
     // Ensure progressRatio is within [0, 1]
    progressRatio = qBound(0.0, progressRatio, 1.0);


    // --- Изменение цвета ---
    QPen pen;
    pen.setWidth(4); // Толщина линии
    pen.setColor(Qt::purple); // Устанавливаем сплошной фиолетовый цвет
    // Можете раскомментировать и настроить этот блок, если хотите градиент вместо сплошного цвета
    /*
    QConicalGradient gradient(outerRect.center(), 0);
    gradient.setColorAt(0.0, Qt::green); // Начало градиента - зеленый
    gradient.setColorAt(1.0, Qt::yellow); // Конец градиента - желтый
    pen.setBrush(gradient);
    */
    painter.setPen(pen);
    // --- Конец изменения цвета ---


    // --- Изменение формы ---
    int numTurns = 5; // Количество витков
    // Увеличиваем количество шагов на виток для плавности, используем 360 шагов на виток (1 шаг на градус)
    double totalStepsPerTurn = 360.0;
    double angleStep = 360.0 / totalStepsPerTurn; // Шаг угла в градусах

    double baseRadius = side * 0.05; // Начальный радиус (5% от размера стороны)
    // Рассчитываем, насколько должен расти радиус за каждый градус,
    // чтобы достичь примерно половины размера виджета (radius = side / 2.0)
    // к концу последнего витка (totalAngle = numTurns * 360.0)
    double totalAngleForMaxRadius = numTurns * 360.0;
    double maxRadius = side / 2.0; // Максимальный радиус спирали
    double radiusGrowthPerDegree = (maxRadius - baseRadius) / totalAngleForMaxRadius;


    QPainterPath spiralPath;
    QPointF center = outerRect.center();

    // Общий угол, который должна покрыть спираль при текущем прогрессе
    double totalAngleCovered = progressRatio * totalAngleForMaxRadius;
    // Количество точек для рисования
    int numberOfPoints = static_cast<int>(totalAngleCovered / angleStep);


    for (int i = 0; i <= numberOfPoints; ++i)
    {
        double currentAngle = i * angleStep; // Текущий угол в градусах
        double radius = baseRadius + currentAngle * radiusGrowthPerDegree; // Радиус растет линейно с углом

        // Преобразуем полярные координаты (radius, currentAngle) в декартовы (x, y)
        double x = center.x() + radius * qCos(qDegreesToRadians(currentAngle));
        double y = center.y() - radius * qSin(qDegreesToRadians(currentAngle));

        if (i == 0)
            spiralPath.moveTo(x, y);
        else
            spiralPath.lineTo(x, y);
    }
    // --- Конец изменения формы ---


    painter.drawPath(spiralPath);

    // Draw progress text
    painter.setPen(Qt::black);
    QFont font = painter.font();
    // Adjust font size based on widget size, but cap it to avoid excessively large text
    int fontSize = static_cast<int>(side * 0.1);
    if (fontSize < 10) fontSize = 10; // Minimum font size
    if (fontSize > 50) fontSize = 50; // Maximum font size
    font.setPointSize(fontSize);
    painter.setFont(font);

    QString progressText = QString("%1%").arg(progressRatio * 100, 0, 'f', (progressRatio == 0.0 || progressRatio == 1.0) ? 0 : 1); // Show 1 decimal place unless 0 or 100
    painter.drawText(outerRect, Qt::AlignCenter, progressText);
}

void SpiralProgressIndicator::wheelEvent(QWheelEvent *event)
{
    // Use pixelDelta for smoother scrolling if available, fallback to angleDelta
    int delta = event->pixelDelta().y();
    if (delta == 0) {
        delta = event->angleDelta().y() / 8; // Adjust sensitivity for angleDelta
    }

    if (delta != 0) {
        // Scale delta based on the range (max - min) to make scrolling meaningful
        // For a default range of 0-100, delta is steps.
        // For a larger range, each scroll tick should represent a smaller fraction of the total range.
        double range = m_maximumValue - m_minimumValue;
        int stepChange = 0;
        if (range > 0) {
             // Adjust sensitivity - a delta of 120 (standard angleDelta) might change progress by ~1-5 units
             // Let's make pixelDelta of ~10 change progress by 1
             stepChange = static_cast<int>(delta / 10.0);
             // Ensure at least a change of 1 if delta is non-zero but small
             if (stepChange == 0 && delta != 0) {
                 stepChange = (delta > 0) ? 1 : -1;
             }
        } else {
            stepChange = (delta > 0) ? 1 : -1; // If range is 0, just increment/decrement
        }

        // Prevent target value from going outside bounds during animation
        int newTarget = m_targetValue + stepChange;
        newTarget = qBound(m_minimumValue, newTarget, m_maximumValue);

        if (newTarget != m_targetValue) {
             m_targetValue = newTarget;
             // Animation timer will handle the rest
        }
        event->accept();
    } else {
        event->ignore();
    }
}


void SpiralProgressIndicator::animateProgress()
{
     if (m_progressValue != m_targetValue)
    {
        int step = (m_targetValue > m_progressValue) ? 1 : -1;

        // Simple easing: move faster when far from target, slower when close
        int diff = qAbs(m_targetValue - m_progressValue);
        if (diff > 10) {
            step *= 2; // Move faster if difference is large
        } else if (diff <= 2) {
            step = (m_targetValue > m_progressValue) ? 1 : -1; // Move slower if difference is small
        } else {
            step = (m_targetValue > m_progressValue) ? 1 : -1; // Normal step
        }

        // Ensure we don't overshoot the target
        if ((step > 0 && m_progressValue + step > m_targetValue) || (step < 0 && m_progressValue + step < m_targetValue)) {
            m_progressValue = m_targetValue;
        } else {
            m_progressValue += step;
        }


        update();

        // Stop the timer if we've reached the target
        if (m_progressValue == m_targetValue) {
            m_animationTimer.stop();
        }
    } else {
        // Timer might still be running if target was reached in the previous step
        m_animationTimer.stop();
    }
}

// Добавьте реализацию для m_animationTimer в конструкторе или в заголовочном файле
// qBound требует #include <QtGlobal>
// qDegreesToRadians, qCos, qSin требуют #include <QtMath>
// QPainterPath требует #include <QPainterPath>
// QWheelEvent требует #include <QWheelEvent>
// QTimer требует #include <QTimer>
// QDebug требует #include <QDebug> (для отладки, можно убрать)
