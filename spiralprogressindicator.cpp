#include "spiralprogressindicator.h"
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>
#include <QMouseEvent> // Added for mouse events
#include <QtMath> // Для qDegreesToRadians, qCos, qSin
#include <QTimer>
#include <QDebug>
#include <QtGlobal> // Для qBound

SpiralProgressIndicator::SpiralProgressIndicator(QWidget *parent)
    : QWidget(parent), m_progressValue(0), m_maximumValue(100), m_minimumValue(0), m_targetValue(0),
      m_isDragging(false), m_initialProgressValue(0) // Инициализация членов для мыши
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // Устанавливаем флаг для отслеживания событий мыши без нажатия кнопки (для mouseMoveEvent)
    setMouseTracking(true);

    connect(&m_animationTimer, &QTimer::timeout, this, &SpiralProgressIndicator::animateProgress);
    m_animationTimer.start(16); // ~60 FPS
}

// *** MISSING DESTRUCTOR DEFINITION ADDED HERE ***
SpiralProgressIndicator::~SpiralProgressIndicator()
{
    // Деструктор: очистка ресурсов, если необходимо.
    // В данном случае QTimer и QPropertyAnimation (если бы использовался)
    // имеют родителя (this) и будут удалены автоматически.
}
// *** END OF ADDED DESTRUCTOR DEFINITION ***


void SpiralProgressIndicator::setProgressValue(int value)
{
    // Ограничиваем значение целевым диапазоном
    int clampedValue = qBound(m_minimumValue, value, m_maximumValue);
    if (clampedValue != m_targetValue) {
        m_targetValue = clampedValue;
        // Если таймер не активен, запускаем его для начала анимации
        if (!m_animationTimer.isActive()) {
            m_animationTimer.start(16);
        }
        // Испускаем сигнал сразу при изменении целевого значения
        emit progressValueChanged(m_targetValue);
    }
}

int SpiralProgressIndicator::progressValue() const
{
    return m_progressValue;
}

void SpiralProgressIndicator::setMaximumValue(int value)
{
    if (m_maximumValue != value) {
        m_maximumValue = value;
        // Убеждаемся, что целевое и текущее значения находятся в новом диапазоне
        m_targetValue = qBound(m_minimumValue, m_targetValue, m_maximumValue);
        m_progressValue = qBound(m_minimumValue, m_progressValue, m_maximumValue);
        emit maximumValueChanged(m_maximumValue);
        // Если текущее значение изменилось или не совпадает с целевым, обновляем и запускаем анимацию
        if (m_progressValue != m_targetValue) {
             if (!m_animationTimer.isActive()) {
                m_animationTimer.start(16);
            }
        } else {
            update(); // Обновляем только отрисовку, если значения не изменились, но диапазон поменялся
        }
    }
}

int SpiralProgressIndicator::maximumValue() const
{
    return m_maximumValue;
}

void SpiralProgressIndicator::setMinimumValue(int value)
{
    if (m_minimumValue != value) {
        m_minimumValue = value;
        // Убеждаемся, что целевое и текущее значения находятся в новом диапазоне
        m_targetValue = qBound(m_minimumValue, m_targetValue, m_maximumValue);
        m_progressValue = qBound(m_minimumValue, m_progressValue, m_maximumValue);
        emit minimumValueChanged(m_minimumValue);
        // Если текущее значение изменилось или не совпадает с целевым, обновляем и запускаем анимацию
        if (m_progressValue != m_targetValue) {
            if (!m_animationTimer.isActive()) {
                m_animationTimer.start(16);
            }
        } else {
            update(); // Обновляем только отрисовку, если значения не изменились, но диапазон поменялся
        }
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

    // Вычисляем центр для рисования сердца
    QPointF center = outerRect.center();
    // Масштабируем сердце, чтобы оно помещалось в виджет
    double scale = side * 0.015; // Коэффициент масштабирования, можно настроить

    // --- Изменение цвета ---
    QPen pen;
    pen.setWidth(4); // Толщина линии
    pen.setColor(Qt::purple); // Устанавливаем сплошной фиолетовый цвет
    painter.setPen(pen);
    // --- Конец изменения цвета ---

    // --- Изменение формы на сердце ---
    QPainterPath heartPath;
    // Используем параметрическое уравнение для сердца
    // t от 0 до 2*PI
    // x = 16 * sin^3(t)
    // y = 13 * cos(t) - 5 * cos(2t) - 2 * cos(3t) - cos(4t)

    // Количество точек для отрисовки сердца
    int numberOfHeartPoints = 360; // Достаточно точек для плавности
    double tStep = 2.0 * M_PI / numberOfHeartPoints;

    // Вычисляем путь для полного сердца
    QPainterPath fullHeartPath;
    for (int i = 0; i <= numberOfHeartPoints; ++i) {
        double t = i * tStep;
        double x = 16.0 * qPow(qSin(t), 3);
        double y = -(13.0 * qCos(t) - 5.0 * qCos(2 * t) - 2.0 * qCos(3 * t) - qCos(4 * t)); // Знак минус, чтобы сердце было направлено вверх

        // Применяем масштабирование и смещение к центру
        QPointF point(center.x() + x * scale, center.y() + y * scale);

        if (i == 0)
            fullHeartPath.moveTo(point);
        else
            fullHeartPath.lineTo(point);
    }

    // Отрисовываем полный контур сердца (опционально, как фон)
    QPen backgroundPen = pen;
    backgroundPen.setColor(Qt::gray); // Цвет фона сердца
    painter.setPen(backgroundPen);
    painter.drawPath(fullHeartPath);

    // Отрисовываем прогресс по контуру сердца
    QPen progressPen = pen;
    progressPen.setColor(Qt::red); // Цвет прогресса
    painter.setPen(progressPen);

    double progressRatio = static_cast<double>(m_progressValue - m_minimumValue) / (m_maximumValue - m_minimumValue);
    // Обработка деления на ноль
    if (m_maximumValue == m_minimumValue) {
        progressRatio = 0.0;
    }
    // Ограничение progressRatio в диапазоне [0, 1]
    progressRatio = qBound(0.0, progressRatio, 1.0);

    // Количество точек для отрисовки прогресса
    int numberOfProgressPoints = static_cast<int>(progressRatio * numberOfHeartPoints);

    QPainterPath progressHeartPath;
    for (int i = 0; i <= numberOfProgressPoints; ++i) {
        double t = i * tStep;
        double x = 16.0 * qPow(qSin(t), 3);
        double y = -(13.0 * qCos(t) - 5.0 * qCos(2 * t) - 2.0 * qCos(3 * t) - qCos(4 * t));

        QPointF point(center.x() + x * scale, center.y() + y * scale);

        if (i == 0)
            progressHeartPath.moveTo(point);
        else
            progressHeartPath.lineTo(point);
    }
    painter.drawPath(progressHeartPath);

    // --- Конец изменения формы ---

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
        // Изменяем целевое значение на основе дельты колесика
        int newTarget = m_targetValue + delta; // Используем дельту напрямую или масштабируем при необходимости
        setProgressValue(newTarget); // Используем setProgressValue для запуска анимации и ограничения
        event->accept();
    } else {
        event->ignore();
    }
}

void SpiralProgressIndicator::mousePressEvent(QMouseEvent *event)
{
    // Начинаем перетаскивание, если нажата левая кнопка мыши
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_lastMousePos = event->pos();
        m_initialProgressValue = m_progressValue; // Сохраняем начальное значение
        event->accept();
    } else {
        event->ignore();
    }
}

void SpiralProgressIndicator::mouseMoveEvent(QMouseEvent *event)
{
    // Обрабатываем движение мыши только при перетаскивании
    if (m_isDragging) {
        QPointF currentPos = event->pos();
        // Вычисляем вертикальное смещение
        double dy = currentPos.y() - m_lastMousePos.y();

        // Масштабируем смещение в изменение значения прогресса
        // Чувствительность можно настроить
        double sensitivity = 0.5; // Настройте это значение
        int valueChange = static_cast<int>(-dy * sensitivity); // Минус, чтобы движение вверх увеличивало значение

        // Вычисляем новое целевое значение
        int newTarget = m_targetValue + valueChange;

        // Обновляем целевое значение с помощью setProgressValue
        setProgressValue(newTarget);

        // Обновляем последнюю позицию мыши для следующего шага
        m_lastMousePos = currentPos;

        event->accept();
    } else {
        event->ignore();
    }
}

void SpiralProgressIndicator::mouseReleaseEvent(QMouseEvent *event)
{
    // Завершаем перетаскивание, если отпущена левая кнопка мыши
    if (event->button() == Qt::LeftButton && m_isDragging) {
        m_isDragging = false;
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

        // Простая анимация: движемся к целевому значению
        // Можно добавить сглаживание, но для простоты пока просто шаг
        m_progressValue += step;

        // Убеждаемся, что не проскакиваем целевое значение
        if ((step > 0 && m_progressValue > m_targetValue) || (step < 0 && m_progressValue < m_targetValue)) {
            m_progressValue = m_targetValue;
        }

        // Испускаем сигнал при каждом шаге анимации для обновления слайдера/поля ввода
        emit progressValueChanged(m_progressValue);

        update();

        // Останавливаем таймер, если достигли целевого значения
        if (m_progressValue == m_targetValue) {
            m_animationTimer.stop();
        }
    } else {
        // Таймер может быть активен, даже если цель достигнута на предыдущем шаге
        m_animationTimer.stop();
    }
}
