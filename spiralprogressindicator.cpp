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
      m_wheelStepSize(5) // Шаг колесика
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus); // Чтобы виджет мог получать события колесика

    // Инициализация объекта анимации
    // Указываем, что анимируем свойство "progressValue" ЭТОГО объекта
    m_animation = new QPropertyAnimation(this, "progressValue", this);
    m_animation->setDuration(250); // Сделаем немного быстрее
    m_animation->setEasingCurve(QEasingCurve::OutQuad); // Сглаживание

    // Подключаем сигнал finished ОДИН РАЗ здесь
    connect(m_animation, &QPropertyAnimation::finished, this, &SpiralProgressIndicator::animationFinished);

    qDebug() << "SpiralProgressIndicator created with min:" << m_minimumValue << "max:" << m_maximumValue << "value:" << m_progressValue;
}

// --- Getters ---
int SpiralProgressIndicator::progressValue() const { return m_progressValue; }
int SpiralProgressIndicator::minimumValue() const { return m_minimumValue; }
int SpiralProgressIndicator::maximumValue() const { return m_maximumValue; }
int SpiralProgressIndicator::wheelStepSize() const { return m_wheelStepSize; }

// --- Вспомогательная функция ---
int SpiralProgressIndicator::clampValue(int value) const
{
    // Используем qBound для ограничения значения между min и max
    int clamped = qBound(m_minimumValue, value, m_maximumValue);
    // qDebug() << "Clamping" << value << "between" << m_minimumValue << "and" << m_maximumValue << "result:" << clamped; // Можно раскомментировать для отладки
    return clamped;
}

// --- Сеттер свойства 'progressValue' (вызывается анимацией) ---
void SpiralProgressIndicator::updateProgressValueAndRepaint(int value)
{
    // Не нужно заново ограничивать, анимация должна давать значения в диапазоне
    // Но на всякий случай оставим
    int newValue = clampValue(value);

    if (m_progressValue != newValue) {
        m_progressValue = newValue;
        // qDebug() << "Animation step: progressValue set to" << m_progressValue;
        emit progressValueChanged(m_progressValue); // Сигналим об ИЗМЕНЕНИИ значения
        update(); // Запрашиваем перерисовку для отображения промежуточного состояния
    }
}

// --- Слот для ЗАПУСКА анимации (вызывается извне) ---
void SpiralProgressIndicator::setProgressValueAnimated(int value)
{
    int targetValue = clampValue(value); // Ограничиваем целевое значение

    // qDebug() << "setProgressValueAnimated called with" << value << ", target:" << targetValue << ", current:" << m_progressValue;

    // Запускаем анимацию, только если целевое значение отличается от текущего
    if (targetValue != m_progressValue) {
         // Проверяем, не анимируем ли мы уже к этому значению
        if (m_animation->state() == QAbstractAnimation::Running &&
            m_animation->endValue().toInt() == targetValue)
        {
            // qDebug() << "Already animating to" << targetValue << ", ignoring.";
            return; // Уже анимируем к нужному значению
        }

        m_animation->stop(); // Останавливаем текущую анимацию (если есть)
        // m_animation->setStartValue(m_progressValue); // QPropertyAnimation сам возьмет текущее значение свойства как стартовое
        m_animation->setEndValue(targetValue); // Устанавливаем конечное значение
        m_animation->start(); // Запускаем анимацию
        // qDebug() << "Starting animation from" << m_progressValue << "to" << targetValue;
    } else {
         // Если значение не изменилось, но анимация могла быть прервана,
         // убедимся, что финальное значение установлено и сигнал отправлен.
         // Это также обработает случай установки начального значения.
         if(m_animation->state() != QAbstractAnimation::Running) {
             // Явно обновим значение, если оно совпадает, но не было установлено (например, при инициализации)
             // или если нужно просто послать сигнал
             updateProgressValueAndRepaint(targetValue);
         }
    }
}

// --- Слот, вызываемый по завершении анимации ---
void SpiralProgressIndicator::animationFinished()
{
    // Анимация вызывает updateProgressValueAndRepaint, который обновляет m_progressValue.
    // Но на всякий случай (если анимация была прервана не на целом шаге или из-за точности),
    // убедимся, что финальное значение точно установлено.
    int finalValue = m_animation->endValue().toInt();
    // qDebug() << "Animation finished. Target was:" << finalValue << "Current is:" << m_progressValue;

    // Обновляем значение и посылаем сигнал, если они отличаются
    // (Это может произойти, если анимация была остановлена)
    updateProgressValueAndRepaint(finalValue);
}

// --- Сеттеры для Min/Max ---
void SpiralProgressIndicator::setMinimumValue(int value)
{
    // qDebug() << "setMinimumValue received:" << value;
    if (m_minimumValue != value) {
        m_minimumValue = value;
        // qDebug() << "Minimum value changed to:" << m_minimumValue;

        // Корректируем максимум, если он стал меньше минимума
        if (m_maximumValue < m_minimumValue) {
            m_maximumValue = m_minimumValue;
            // qDebug() << "Adjusted maximumValue to minimum:" << m_maximumValue;
            emit maximumValueChanged(m_maximumValue);
        }

        // Корректируем текущее значение БЕЗ АНИМАЦИИ
        if (m_progressValue < m_minimumValue) {
            // qDebug() << "Adjusting progressValue to minimum:" << m_minimumValue;
            // Используем прямой сеттер, чтобы не запустить анимацию
            updateProgressValueAndRepaint(m_minimumValue);
        }

        emit minimumValueChanged(m_minimumValue);
        update(); // Запрашиваем перерисовку (диапазон мог измениться)
    }
}

void SpiralProgressIndicator::setMaximumValue(int value)
{
    // qDebug() << "setMaximumValue received:" << value;
    if (m_maximumValue != value) {
        m_maximumValue = value;
        // qDebug() << "Maximum value changed to:" << m_maximumValue;

        // Корректируем минимум, если он стал больше максимума
        if (m_minimumValue > m_maximumValue) {
            m_minimumValue = m_maximumValue;
            // qDebug() << "Adjusted minimumValue to maximum:" << m_minimumValue;
            emit minimumValueChanged(m_minimumValue);
        }

        // Корректируем текущее значение БЕЗ АНИМАЦИИ
        if (m_progressValue > m_maximumValue) {
            // qDebug() << "Adjusting progressValue to maximum:" << m_maximumValue;
            // Используем прямой сеттер, чтобы не запустить анимацию
            updateProgressValueAndRepaint(m_maximumValue);
        }

        emit maximumValueChanged(m_maximumValue);
        update(); // Запрашиваем перерисовку
    }
}

// --- Сеттер для Wheel Step ---
void SpiralProgressIndicator::setWheelStepSize(int size)
{
    if (m_wheelStepSize != size && size > 0) { // Шаг должен быть положительным
        m_wheelStepSize = size;
        // qDebug() << "Wheel step size changed to:" << m_wheelStepSize;
        emit wheelStepSizeChanged(m_wheelStepSize);
    }
}

// --- Отрисовка ---
void SpiralProgressIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // Включаем сглаживание

    int side = qMin(width(), height()); // Размер виджета
    QPointF center = rect().center(); // Центр виджета
    qreal devicePixelRatio = painter.device()->devicePixelRatioF(); // Для HiDPI

    // Параметры спирали
    qreal penWidth = side * 0.02 * devicePixelRatio; // Толщина линии с учетом DPI
    qreal startRadius = side * 0.1;   // Начальный радиус
    qreal endRadius = side * 0.4;     // Конечный радиус
    int numTurns = 4;                 // Количество витков
    qreal angleStepDegrees = 1.0;     // Шаг угла для построения пути (в градусах)

    qreal totalAngleDegrees = numTurns * 360.0; // Общий угол спирали

    // Рассчитываем прогресс (от 0.0 до 1.0)
    qreal progressRatio = 0.0;
    if (m_maximumValue > m_minimumValue) { // Избегаем деления на ноль
        progressRatio = static_cast<qreal>(m_progressValue - m_minimumValue) / (m_maximumValue - m_minimumValue);
    }
    qreal currentAngleDegrees = progressRatio * totalAngleDegrees; // Текущий угол прогресса

    // --- Строим путь для полной спирали (фон) ---
    QPainterPath fullSpiralPath;
    qreal startX = center.x() + startRadius;
    qreal startY = center.y();
    fullSpiralPath.moveTo(startX, startY); // Начинаем справа

    for (qreal angleDeg = angleStepDegrees; angleDeg <= totalAngleDegrees; angleDeg += angleStepDegrees) {
        qreal currentRatio = angleDeg / totalAngleDegrees;
        qreal currentRadius = startRadius + (endRadius - startRadius) * currentRatio;
        qreal angleRad = qDegreesToRadians(angleDeg); // Угол в радианах
        qreal x = center.x() + currentRadius * qCos(angleRad);
        qreal y = center.y() + currentRadius * qSin(angleRad);
        fullSpiralPath.lineTo(x, y);
    }

    // --- Строим путь для части спирали (прогресс) ---
    QPainterPath progressSpiralPath;
    if (currentAngleDegrees > 0) {
        progressSpiralPath.moveTo(startX, startY); // Начинаем справа
        for (qreal angleDeg = angleStepDegrees; angleDeg <= currentAngleDegrees; angleDeg += angleStepDegrees) {
            qreal currentRatio = angleDeg / totalAngleDegrees;
            // Защита от деления на ноль, если totalAngleDegrees = 0 (хотя не должно быть)
             if (totalAngleDegrees > 1e-6) {
                 currentRatio = angleDeg / totalAngleDegrees;
             } else {
                 currentRatio = 0;
             }
            qreal currentRadius = startRadius + (endRadius - startRadius) * currentRatio;
            qreal angleRad = qDegreesToRadians(angleDeg);
            qreal x = center.x() + currentRadius * qCos(angleRad);
            qreal y = center.y() + currentRadius * qSin(angleRad);
            progressSpiralPath.lineTo(x, y);
        }
         // Добавляем последнюю точку точно на currentAngleDegrees
         if (currentAngleDegrees > angleStepDegrees) {
            qreal currentRatio = currentAngleDegrees / totalAngleDegrees;
             qreal currentRadius = startRadius + (endRadius - startRadius) * currentRatio;
             qreal angleRad = qDegreesToRadians(currentAngleDegrees);
             qreal x = center.x() + currentRadius * qCos(angleRad);
             qreal y = center.y() + currentRadius * qSin(angleRad);
             progressSpiralPath.lineTo(x,y);
         }
    }


    // --- Рисуем фон спирали ---
    QColor backgroundColor = palette().color(QPalette::Disabled, QPalette::WindowText); // Серый цвет из палитры
    painter.setPen(QPen(backgroundColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(fullSpiralPath);

    // --- Рисуем прогресс спирали ---
    QColor progressColor = palette().color(QPalette::Highlight); // Цвет выделения из палитры (обычно синий)
    painter.setPen(QPen(progressColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(progressSpiralPath);

    // --- Рисуем текст в центре ---
    painter.setPen(palette().color(QPalette::WindowText)); // Цвет текста из палитры
    QFont font = painter.font();
    // Подбираем размер шрифта относительно размера виджета, но не слишком большой
    int fontSize = qBound(10, static_cast<int>(side * 0.1), 40); // Ограничим размер шрифта
    font.setPointSize(fontSize);
    painter.setFont(font);

    // Формируем текст (например, "50%")
    QString progressText = QString::number(qRound(progressRatio * 100)) + "%";

    // Вычисляем прямоугольник для текста и центрируем его
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(progressText);
    textRect.moveCenter(center.toPoint());

    // Рисуем текст
    painter.drawText(textRect, Qt::AlignCenter, progressText);
}

// --- Обработка колесика мыши ---
void SpiralProgressIndicator::wheelEvent(QWheelEvent *event)
{
    // Проверяем, есть ли вертикальная прокрутка
    if (event->angleDelta().y() == 0) {
        event->ignore(); // Игнорируем горизонтальную прокрутку или если нет дельты
        return;
    }

    // Получаем "шаги" колесика (обычно 1 или -1 для стандартного шага в 120)
    int steps = event->angleDelta().y() / 120;

    // Вычисляем новое значение
    int newValue = m_progressValue + steps * m_wheelStepSize;

    // qDebug() << "Wheel event steps:" << steps << "wheelStepSize:" << m_wheelStepSize << "Calculated newValue:" << newValue;

    // Запускаем анимацию к новому значению
    setProgressValueAnimated(newValue);

    event->accept(); // Сообщаем, что событие обработано
}
