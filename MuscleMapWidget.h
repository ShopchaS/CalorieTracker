/**
 * @file MuscleMapWidget.h
 * @brief Кастомный виджет интерактивной карты мышц.
 * @details Описывает класс для визуального выбора групп мышц на графическом 
 * макете тела (передний и задний вид).
 */
#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QMap>
#include <QPainterPath>
#include <QStringList>
#include <QPushButton>
#include <QColor>

/**
 * @brief Виджет интерактивной карты мышц тела человека.
 * @details Позволяет визуально выбирать группы мышц для ручного планирования тренировки.
 */
class MuscleMapWidget : public QWidget {
    Q_OBJECT
public:
    /**
     * @brief Конструктор виджета карты мышц.
     * @param parent Родительский виджет (по умолчанию nullptr).
     */
    explicit MuscleMapWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Получить список названий всех выбранных пользователем групп мышц.
     * @return QStringList Список строк с названиями мышц.
     */
    QStringList getSelectedGroups() const;
    
    /**
     * @brief Сбрасывает выделение со всех мышечных зон.
     */
    void clearSelection();

protected:
    /**
     * @brief Обработчик отрисовки графики (картинки тела, полигонов выделения и текста).
     * @param event Указатель на событие отрисовки.
     */
    void paintEvent(QPaintEvent *event) override;
    
    /**
     * @brief Обработчик кликов мыши для переключения статуса выделения мышц.
     * @param event Указатель на событие мыши.
     */
    void mousePressEvent(QMouseEvent *event) override;
    
    /**
     * @brief Обработчик изменения размеров виджета для адаптации кнопок управления.
     * @param event Указатель на событие изменения размера.
     */
    void resizeEvent(QResizeEvent *event) override;

private:
    /**
     * @brief Структура, описывающая отдельную мышечную зону на карте (хитбокс).
     */
    struct MuscleZone {
        QPainterPath path;    /**< Геометрическая форма зоны выделения */
        bool selected = false;/**< Флаг: выбрана ли зона пользователем */
        bool isFront = true;  /**< Флаг принадлежности зоны к переднему или заднему виду */
        QColor fillColor;     /**< Уникальный цвет, которым закрашивается зона при клике */
    };

    QMap<QString, MuscleZone> zones; /**< Карта всех мышечных зон, где ключ - название группы мышц */
    QPixmap frontPixmap;             /**< Изображение тела спереди */
    QPixmap backPixmap;              /**< Изображение тела сзади */
    bool showFront = true;           /**< Флаг, определяющий, какой вид тела сейчас отображается */

    QPushButton *frontBtn;           /**< Кнопка переключения на вид спереди */
    QPushButton *backBtn;            /**< Кнопка переключения на вид сзади */

    /**
     * @brief Инициализирует координаты, цвета и пути (QPainterPath) для всех мышечных зон.
     */
    void initZones();
};