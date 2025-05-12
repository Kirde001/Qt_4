#include "spiralprogressindicator.h"
#include <QPainterPath> // Требуется для рисования сложных фигур, таких как спирали
#include <QtMath>       // Требуется для математических функций, например qDegreesToRadians
#include <QFontMetrics> // Требуется для расчета размера текста
#include <QRectF>       // Для работы с прямоугольниками с плавающей точкой
#include <QRadialGradient> // Можно использовать для градиентной заливки, если нужно (не реализовано в текущем коде)

SpiralProgressIndicator::SpiralProgressIndicator(QWidget *parent)
    : QWidget(parent),
      m_progressValue(0),      // Начальное значение прогресса
      m_minimumValue(0),       // Начальный минимум
      m_maximumValue(100),     // Начальный максимум
      m_wheelStepSize(5)       // Шаг изменения значения колесиком мыши
{
    // Устанавливаем политику размеров, чтобы виджет мог расширяться
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(100, 100); // Устанавливаем минимальный размер, чтобы спираль была видна

    // Инициализируем объект анимации
    // Анимируем свойство "progressValue" для текущего объекта (this)
    m_animation = new QPropertyAnimation(this, "progressValue", this);
    m_animation->setDuration(300); // Длительность анимации в миллисекундах (меньше для быстрого отклика)
    m_animation->setEasingCurve(QEasingCurve::OutQuad); // Кривая сглаживания анимации (можно попробовать другие)

    qDebug() << "SpiralProgressIndicator создан с мин:" << m_minimumValue << "макс:" << m_maximumValue << "значение:" << m_progressValue;
}

SpiralProgressIndicator::~SpiralProgressIndicator()
{
    // Объект m_animation будет удален автоматически, так как у него установлен родитель (this)
    // delete m_animation; // Не требуется, если установлен родитель
}

// Геттер для progressValue
int SpiralProgressIndicator::progressValue() const
{
    return m_progressValue;
}

// Сеттер для progressValue - запускает анимацию изменения значения
void SpiralProgressIndicator::setProgressValue(int value)
{
    qDebug() << "setProgressValue получено:" << value << "Текущий мин:" << m_minimumValue << "макс:" << m_maximumValue << "текущее значение:" << m_progressValue;

    // Ограничиваем целевое значение в пределах [minimumValue, maximumValue]
    int targetValue = clampValue(value);

    // Проверяем, изменится ли значение
    if (m_progressValue != targetValue) {
        qDebug() << "Анимируем от" << m_progressValue << "до" << targetValue;

        // Останавливаем текущую анимацию, если она есть
        m_animation->stop();
        // Устанавливаем начальное значение анимации (текущее значение)
        m_animation->setStartValue(m_progressValue);
        // Устанавливаем конечное значение анимации (целевое значение)
        m_animation->setEndValue(targetValue);

        // Соединяем сигнал finished анимации с нашим слотом animationFinished
        // Сначала отключаем предыдущие соединения, чтобы избежать дублирования вызовов
        disconnect(m_animation, &QPropertyAnimation::finished, this, &SpiralProgressIndicator::animationFinished);
        connect(m_animation, &QPropertyAnimation::finished, this, &SpiralProgressIndicator::animationFinished);

        // Запускаем анимацию
        m_animation->start();
    } else {
        qDebug() << "Значение не изменилось, анимация не требуется.";
    }
}

// Слот, вызываемый по завершении анимации
void SpiralProgressIndicator::animationFinished()
{
    // Анимация сама обновляет свойство "progressValue" по мере выполнения,
    // вызывая setProgressValue с промежуточными значениями.
    // Этот слот вызывается только по окончании анимации.
    // Мы явно устанавливаем конечное значение и испускаем сигнал, чтобы гарантировать
    // точное конечное состояние и уведомить связанные объекты (например, слайдер).
    m_progressValue = m_animation->endValue().toInt();
    qDebug() << "Анимация завершена. Конечное значение:" << m_progressValue;
    emit progressValueChanged(m_progressValue); // Испускаем сигнал с конечным значением
    update(); // Запрашиваем финальную перерисовку
}

// Геттер для minimumValue
int SpiralProgressIndicator::minimumValue() const
{
    return m_minimumValue;
}

// Сеттер для minimumValue
void SpiralProgressIndicator::setMinimumValue(int value)
{
    qDebug() << "setMinimumValue получено:" << value;
    if (m_minimumValue != value) {
        m_minimumValue = value;
        qDebug() << "Минимальное значение изменено на:" << m_minimumValue;
        // Убеждаемся, что текущее и максимальное значения находятся в новом диапазоне
        if (m_progressValue < m_minimumValue) {
            // Прямо устанавливаем progressValue без анимации при изменении диапазона
            m_progressValue = m_minimumValue;
            qDebug() << "progressValue скорректировано до минимума:" << m_progressValue;
            emit progressValueChanged(m_progressValue); // Испускаем сигнал вручную
        }
        if (m_maximumValue < m_minimumValue) {
            m_maximumValue = m_minimumValue;
             qDebug() << "maximumValue скорректировано до минимума:" << m_maximumValue;
            emit maximumValueChanged(m_maximumValue); // Испускаем сигнал вручную
        }
        emit minimumValueChanged(m_minimumValue); // Испускаем сигнал об изменении минимума
        update(); // Запрашиваем перерисовку
    } else {
        qDebug() << "Минимальное значение не изменилось.";
    }
}

// Геттер для maximumValue
int SpiralProgressIndicator::maximumValue() const
{
    return m_maximumValue;
}

// Сеттер для maximumValue
void SpiralProgressIndicator::setMaximumValue(int value)
{
    qDebug() << "setMaximumValue получено:" << value;
    if (m_maximumValue != value) {
        m_maximumValue = value;
        qDebug() << "Максимальное значение изменено на:" << m_maximumValue;
         // Убеждаемся, что текущее и минимальное значения находятся в новом диапазоне
        if (m_progressValue > m_maximumValue) {
             // Прямо устанавливаем progressValue без анимации при изменении диапазона
            m_progressValue = m_maximumValue;
            qDebug() << "progressValue скорректировано до максимума:" << m_progressValue;
            emit progressValueChanged(m_progressValue); // Испускаем сигнал вручную
        }
        if (m_minimumValue > m_maximumValue) {
            m_minimumValue = m_maximumValue;
             qDebug() << "minimumValue скорректировано до максимума:" << m_minimumValue;
            emit minimumValueChanged(m_minimumValue); // Испускаем сигнал вручную
        }
        emit maximumValueChanged(m_maximumValue); // Испускаем сигнал об изменении максимума
        update(); // Запрашиваем перерисовку
    } else {
         qDebug() << "Максимальное значение не изменилось.";
    }
}

// Геттер для wheelStepSize
int SpiralProgressIndicator::wheelStepSize() const
{
    return m_wheelStepSize;
}

// Сеттер для wheelStepSize
void SpiralProgressIndicator::setWheelStepSize(int size)
{
    if (m_wheelStepSize != size) {
        m_wheelStepSize = size;
        qDebug() << "Шаг колесика изменен на:" << m_wheelStepSize;
        emit wheelStepSizeChanged(m_wheelStepSize); // Испускаем сигнал об изменении шага колесика
    }
}

// Вспомогательная функция для ограничения значения
int SpiralProgressIndicator::clampValue(int value) const
{
    // qBound возвращает значение, ограниченное между lower и upper
    int clamped = qBound(m_minimumValue, value, m_maximumValue);
    qDebug() << "Ограничиваем" << value << "между" << m_minimumValue << "и" << m_maximumValue << "Результат:" << clamped;
    return clamped;
}

// Обработчик события перерисовки виджета
void SpiralProgressIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); // Этот аргумент не используется, помечаем его как неиспользуемый

    QPainter painter(this);
    // Включаем сглаживание для более плавных линий
    painter.setRenderHint(QPainter::Antialiasing);
    // QPainter::HighQualityAntialiasing устарел, используем Antialiasing

    // Вычисляем размер квадратной области для спирали
    int side = qMin(width(), height());
    // Вычисляем центр виджета
    QPointF center = rect().center();

    // Определяем параметры для спирали (можно настраивать)
    qreal startRadius = side * 0.1; // Начальный радиус спирали (относительно размера виджета)
    qreal endRadius = side * 0.4;  // Конечный радиус спирали (около половины стороны)
    int numTurns = 5;              // Количество витков спирали
    qreal angleStep = 1.0;         // Шаг угла для отрисовки (меньше -> плавнее, но медленнее)

    // Вычисляем общий угол, покрываемый всей спиралью
    qreal totalAngle = numTurns * 360.0;

    // Вычисляем текущий угол в зависимости от прогресса
    qreal progressRatio = 0.0;
    if (m_maximumValue > m_minimumValue) {
        progressRatio = static_cast<qreal>(m_progressValue - m_minimumValue) / (m_maximumValue - m_minimumValue);
    }
    // Убеждаемся, что progressRatio находится в пределах [0, 1]
    progressRatio = qBound(0.0, progressRatio, 1.0);

    qreal currentAngle = progressRatio * totalAngle;

    // Создаем путь для всей спирали (фон)
    QPainterPath fullSpiralPath;
    // Начинаем спираль справа от центра (угол 0)
    fullSpiralPath.moveTo(center.x() + startRadius, center.y());

    // Строим полную спираль по точкам
    for (qreal angle = 0; angle <= totalAngle; angle += angleStep) {
        // Вычисляем текущий радиус (линейно увеличивается от startRadius до endRadius)
        qreal currentRadius = startRadius + (endRadius - startRadius) * (angle / totalAngle);

        // Преобразуем полярные координаты (радиус, угол) в декартовы (x, y)
        // Угол должен быть в радианах для тригонометрических функций
        qreal x = center.x() + currentRadius * qCos(qDegreesToRadians(angle));
        qreal y = center.y() + currentRadius * qSin(qDegreesToRadians(angle));

        fullSpiralPath.lineTo(x, y);
    }

    // Создаем путь для части спирали, показывающей прогресс
    QPainterPath progressSpiralPath;
     if (currentAngle > 0) {
        // Начинаем прогресс спираль также справа от центра
        progressSpiralPath.moveTo(center.x() + startRadius, center.y());

        // Строим прогресс спираль до текущего угла
        for (qreal angle = 0; angle <= currentAngle; angle += angleStep) {
            qreal currentRadius = startRadius + (endRadius - startRadius) * (angle / totalAngle);
            qreal x = center.x() + currentRadius * qCos(qDegreesToRadians(angle));
            qreal y = center.y() + currentRadius * qSin(qDegreesToRadians(angle));
            progressSpiralPath.lineTo(x, y);
        }
     }

    // Рисуем путь полной спирали (фон)
    // Толщина линии относительно размера виджета, скругленные концы и соединения
    painter.setPen(QPen(Qt::gray, side * 0.02, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(fullSpiralPath);

    // Рисуем путь прогресс спирали (передний план)
    painter.setPen(QPen(Qt::blue, side * 0.02, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(progressSpiralPath);

    // Рисуем текст текущего прогресса в центре
    painter.setPen(Qt::black); // Цвет текста
    QFont font = painter.font();
    int fontSize = static_cast<int>(side * 0.1); // Размер шрифта относительно размера виджета
    font.setPointSize(fontSize);
    painter.setFont(font);

    // Форматируем текст прогресса (например, "50%")
    // progressRatio * 100 - процентное значение
    // 0 - минимальная ширина поля (без выравнивания по ширине)
    // 'f' - формат с плавающей точкой
    // 0 - количество знаков после запятой (отображаем как целое число)
    QString progressText = QString("%1%").arg(progressRatio * 100, 0, 'f', 0);

    // Вычисляем прямоугольник для текста для центрирования
    QFontMetrics fm(font);
    // BoundingRect возвращает размер прямоугольника, который занимает текст
    QRect textRect = fm.boundingRect(progressText);
    // Перемещаем центр текстового прямоугольника в центр виджета
    textRect.moveCenter(center.toPoint());

    // Рисуем текст по центру
    painter.drawText(textRect, Qt::AlignCenter, progressText);
}

// Обработчик события вращения колесика мыши
void SpiralProgressIndicator::wheelEvent(QWheelEvent *event)
{
    // Вычисляем изменение значения на основе дельты колесика
    // Стандартная дельта колесика - 120 на один "щелчок"
    int delta = event->angleDelta().y() / 120;
    // Вычисляем новое целевое значение с учетом шага колесика
    int newValue = m_progressValue + delta * m_wheelStepSize;

    qDebug() << "Событие колесика, дельта:" << delta << "шаг колесика:" << m_wheelStepSize << "Вычислено новое значение:" << newValue;

    // Устанавливаем новое значение. Это вызовет сеттер setProgressValue,
    // который запустит анимацию.
    setProgressValue(newValue);

    // Принимаем событие, чтобы оно не обрабатывалось далее родительскими виджетами
    event->accept();
}
