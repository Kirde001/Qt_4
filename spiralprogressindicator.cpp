#include "spiralprogressindicator.h"
#include <QPainterPath> // Required for drawing complex shapes like spirals
#include <QDebug>       // For debugging output
#include <QtMath>       // Required for qDegreesToRadians
#include <QFontMetrics> // Required for text size calculation

SpiralProgressIndicator::SpiralProgressIndicator(QWidget *parent)
    : QWidget(parent),
      m_progressValue(0),
      m_minimumValue(0),
      m_maximumValue(100),
      m_wheelStepSize(5) // Изменяем шаг колесика на 5
{
    // Set size policy to allow expanding
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Initialize animation object
    m_animation = new QPropertyAnimation(this, "progressValue", this);
    m_animation->setDuration(300); // Уменьшим длительность анимации для более быстрого отклика
    m_animation->setEasingCurve(QEasingCurve::OutQuad); // Изменим кривую сглаживания

    qDebug() << "SpiralProgressIndicator created with min:" << m_minimumValue << "max:" << m_maximumValue << "value:" << m_progressValue;
}

SpiralProgressIndicator::~SpiralProgressIndicator()
{
    // Animation is parented, so it will be deleted automatically.
    // delete m_animation; // Not needed if parented
}

// Getter for progressValue
int SpiralProgressIndicator::progressValue() const
{
    return m_progressValue;
}

// Setter for progressValue - starts the animation
void SpiralProgressIndicator::setProgressValue(int value)
{
    qDebug() << "setProgressValue received:" << value << "Current min:" << m_minimumValue << "max:" << m_maximumValue;

    // Clamp the target value
    int targetValue = clampValue(value);

    if (m_progressValue != targetValue) {
        qDebug() << "Animating from" << m_progressValue << "to" << targetValue;
        m_animation->stop(); // Stop any ongoing animation
        m_animation->setStartValue(m_progressValue); // Start from current value
        m_animation->setEndValue(targetValue);       // Animate to the target value

        // Connect finished signal to update the final value and emit the signal
        // Disconnect previous connections first to avoid multiple connections
        disconnect(m_animation, &QPropertyAnimation::finished, this, &SpiralProgressIndicator::animationFinished);
        connect(m_animation, &QPropertyAnimation::finished, this, &SpiralProgressIndicator::animationFinished);

        m_animation->start();
    } else {
         qDebug() << "Value did not change significantly, no animation needed.";
    }
}

// Slot to handle animation finished
void SpiralProgressIndicator::animationFinished()
{
    // The animation updates the 'progressValue' property directly,
    // which in turn calls setProgressValue with the animated value.
    // The final value is set by the animation. We just need to ensure
    // the signal is emitted at the end if needed, but setProgressValue
    // already handles the initial value change. The animation itself
    // triggers updates.
    // However, if the animation was stopped prematurely, the final value
    // might not be exactly the target. We can ensure it here.
    m_progressValue = m_animation->endValue().toInt();
    qDebug() << "Animation finished. Final value:" << m_progressValue;
    emit progressValueChanged(m_progressValue); // Ensure signal is emitted at the end
    update(); // Request final repaint
}


// Getter for minimumValue
int SpiralProgressIndicator::minimumValue() const
{
    return m_minimumValue;
}

// Setter for minimumValue
void SpiralProgressIndicator::setMinimumValue(int value)
    {
    qDebug() << "setMinimumValue received:" << value;
    if (m_minimumValue != value) {
        m_minimumValue = value;
        qDebug() << "Minimum value changed to:" << m_minimumValue;
        // Ensure current value and max value are within the new range
        if (m_progressValue < m_minimumValue) {
            // Directly set progressValue without animation to avoid issues with range change during animation
            m_progressValue = m_minimumValue;
            qDebug() << "Adjusted progressValue to minimum:" << m_progressValue;
            emit progressValueChanged(m_progressValue); // Emit signal manually
        }
        if (m_maximumValue < m_minimumValue) {
            m_maximumValue = m_minimumValue;
             qDebug() << "Adjusted maximumValue to minimum:" << m_maximumValue;
            emit maximumValueChanged(m_maximumValue); // Emit signal manually
        }
        emit minimumValueChanged(m_minimumValue); // Emit signal
        update(); // Request repaint
    } else {
        qDebug() << "Minimum value did not change.";
    }
}

// Getter for maximumValue
int SpiralProgressIndicator::maximumValue() const
{
    return m_maximumValue;
}

// Setter for maximumValue
void SpiralProgressIndicator::setMaximumValue(int value)
{
    qDebug() << "setMaximumValue received:" << value;
    if (m_maximumValue != value) {
        m_maximumValue = value;
        qDebug() << "Maximum value changed to:" << m_maximumValue;
         // Ensure current value and min value are within the new range
        if (m_progressValue > m_maximumValue) {
             // Directly set progressValue without animation
            m_progressValue = m_maximumValue;
            qDebug() << "Adjusted progressValue to maximum:" << m_progressValue;
            emit progressValueChanged(m_progressValue); // Emit signal manually
        }
        if (m_minimumValue > m_maximumValue) {
            m_minimumValue = m_maximumValue;
            qDebug() << "Adjusted minimumValue to maximum:" << m_minimumValue;
            emit minimumValueChanged(m_minimumValue); // Emit signal manually
        }
        emit maximumValueChanged(m_maximumValue); // Emit signal
        update(); // Request repaint
    } else {
         qDebug() << "Maximum value did not change.";
    }
}

// Getter for wheelStepSize
int SpiralProgressIndicator::wheelStepSize() const
{
    return m_wheelStepSize;
}

// Setter for wheelStepSize
void SpiralProgressIndicator::setWheelStepSize(int size)
{
    if (m_wheelStepSize != size) {
        m_wheelStepSize = size;
        qDebug() << "Wheel step size changed to:" << m_wheelStepSize;
        emit wheelStepSizeChanged(m_wheelStepSize); // Emit signal
    }
}


// Helper function to clamp value
int SpiralProgressIndicator::clampValue(int value) const
{
    int clamped = qBound(m_minimumValue, value, m_maximumValue);
    qDebug() << "Clamping" << value << "between" << m_minimumValue << "and" << m_maximumValue << "result:" << clamped;
    return clamped;
}


void SpiralProgressIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    // Use Antialiasing instead of HighQualityAntialiasing
    painter.setRenderHint(QPainter::Antialiasing);
    // painter.setRenderHint(QPainter::HighQualityAntialiasing); // Deprecated

    // Calculate the size of the drawable area
    int side = qMin(width(), height());
    // Calculate the center of the widget
    QPointF center = rect().center();

    // Define parameters for the spiral
    qreal startRadius = side * 0.1; // Starting radius of the spiral
    qreal endRadius = side * 0.4;   // Ending radius of the spiral (approx half of the side)
    int numTurns = 5;               // Number of turns in the spiral
    qreal angleStep = 1.0;          // Angle step for drawing (smaller means smoother)

    // Calculate the total angle covered by the spiral
    qreal totalAngle = numTurns * 360.0;

    // Calculate the current angle based on progress
    qreal progressRatio = static_cast<qreal>(m_progressValue - m_minimumValue) / (m_maximumValue - m_minimumValue);
    if (m_maximumValue == m_minimumValue) progressRatio = 0; // Avoid division by zero
    qreal currentAngle = progressRatio * totalAngle;

    // Create the path for the full spiral
    QPainterPath fullSpiralPath;
    // Start the spiral from the right side
    fullSpiralPath.moveTo(center.x() + startRadius, center.y());

    for (qreal angle = 0; angle <= totalAngle; angle += angleStep) {
        // Calculate the current radius based on the angle (linear increase)
        qreal currentRadius = startRadius + (endRadius - startRadius) * (angle / totalAngle);

        // Convert polar coordinates (radius, angle) to Cartesian coordinates (x, y)
        // Angle needs to be in radians for trigonometric functions
        qreal x = center.x() + currentRadius * qCos(qDegreesToRadians(angle));
        qreal y = center.y() + currentRadius * qSin(qDegreesToRadians(angle));

        fullSpiralPath.lineTo(x, y);
    }

    // Create the path for the progress part of the spiral
    QPainterPath progressSpiralPath;
     if (currentAngle > 0) {
        // Start the progress spiral from the right side
        progressSpiralPath.moveTo(center.x() + startRadius, center.y());

        for (qreal angle = 0; angle <= currentAngle; angle += angleStep) {
            qreal currentRadius = startRadius + (endRadius - startRadius) * (angle / totalAngle);
            qreal x = center.x() + currentRadius * qCos(qDegreesToRadians(angle));
            qreal y = center.y() + currentRadius * qSin(qDegreesToRadians(angle));
            progressSpiralPath.lineTo(x, y);
        }
     }


    // Draw the full spiral path (background)
    painter.setPen(QPen(Qt::gray, side * 0.02, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)); // Pen style
    painter.drawPath(fullSpiralPath);

    // Draw the progress spiral path (foreground)
    painter.setPen(QPen(Qt::blue, side * 0.02, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)); // Pen style
    painter.drawPath(progressSpiralPath);

    // Draw the current progress text in the center
    painter.setPen(Qt::black); // Text color
    QFont font = painter.font();
    int fontSize = static_cast<int>(side * 0.1); // Font size relative to widget size
    font.setPointSize(fontSize);
    painter.setFont(font);

    // Format the progress text (e.g., "50%")
    QString progressText = QString("%1%").arg(progressRatio * 100, 0, 'f', 0); // Display as integer percentage

    // Calculate text rectangle for centering
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(progressText);
    textRect.moveCenter(center.toPoint()); // Center the text rectangle

    painter.drawText(textRect, Qt::AlignCenter, progressText);
}

void SpiralProgressIndicator::wheelEvent(QWheelEvent *event)
{
    // Calculate the change in value based on wheel delta
    int delta = event->angleDelta().y() / 120; // Standard wheel delta is 120 per click
    // Calculate the new target value using the step size
    int newValue = m_progressValue + delta * m_wheelStepSize;

    qDebug() << "Wheel event delta:" << delta << "wheelStepSize:" << m_wheelStepSize << "Calculated newValue:" << newValue;

    // Set the new value, which will trigger the animation
    setProgressValue(newValue);

    // Accept the event to prevent further propagation
    event->accept();
}
