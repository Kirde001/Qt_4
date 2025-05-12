#ifndef SPIRALPROGRESSINDICATOR_H
#define SPIRALPROGRESSINDICATOR_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QPropertyAnimation> // Required for animation
#include <QEasingCurve>     // Required for animation easing

class SpiralProgressIndicator : public QWidget
{
    Q_OBJECT

    // Define progressValue as a Q_PROPERTY for animation
    Q_PROPERTY(int progressValue READ progressValue WRITE setProgressValue NOTIFY progressValueChanged)
    // Define min/max as Q_PROPERTY as well, though not strictly needed for animation
    Q_PROPERTY(int minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)
    Q_PROPERTY(int maximumValue READ maximumValue WRITE setMaximumValue NOTIFY maximumValueChanged)
    // Add a property for wheel step size
    Q_PROPERTY(int wheelStepSize READ wheelStepSize WRITE setWheelStepSize NOTIFY wheelStepSizeChanged)


public:
    explicit SpiralProgressIndicator(QWidget *parent = nullptr);
    ~SpiralProgressIndicator(); // Destructor to clean up animation

    // Getters for properties
    int progressValue() const;
    int minimumValue() const;
    int maximumValue() const;
    int wheelStepSize() const; // Getter for wheelStepSize

signals:
    // Signal emitted when progressValue changes
    void progressValueChanged(int value);
    // Signals for min/max changes (optional but good practice)
    void minimumValueChanged(int value);
    void maximumValueChanged(int value);
    // Signal for wheelStepSize changes
    void wheelStepSizeChanged(int size);

public slots:
    // Setter for progressValue - will trigger animation
    void setProgressValue(int value);
    // Setters for min/max
    void setMinimumValue(int value);
    void setMaximumValue(int value);
    void setWheelStepSize(int size); // Setter for wheelStepSize


private slots: // Declare animationFinished as a private slot
    void animationFinished();


protected:
    // Override paintEvent for custom drawing
    void paintEvent(QPaintEvent *event) override;
    // Override wheelEvent for mouse wheel interaction
    void wheelEvent(QWheelEvent *event) override;

private:
    // Private members for property values
    int m_progressValue;
    int m_minimumValue;
    int m_maximumValue;
    int m_wheelStepSize; // Member for wheel step size

    // Animation object
    QPropertyAnimation *m_animation;

    // Helper function to clamp value within min/max range
    int clampValue(int value) const;
};

#endif // SPIRALPROGRESSINDICATOR_H
