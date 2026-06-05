/**
 * @file WorkoutCalculator.h
 * @brief Калькулятор калорий и генератор тренировочных планов.
 * @details Объявляет статические методы для расчета BMR и динамического 
 * формирования списка упражнений в зависимости от состояния здоровья пользователя.
 */
#pragma once
#include <QString>
#include <vector>
#include <QDate>
#include <QStringList>
#include "CoreData.h"

/**
 * @brief Структура для описания одного упражнения.
 */
struct Exercise { 
    QString name;  /**< Название упражнения */
    QString group; /**< Группа мышц */
};

/**
 * @brief Класс-калькулятор тренировок.
 */
class WorkoutCalculator {
public:
    static const std::vector<Exercise> EXERCISE_DB; /**< База упражнений */

    /**
     * @brief Расчет нормы калорий.
     * @param u Пользователь.
     * @return int Суточная норма.
     */
    static int calculateDailyCalories(const User& u);

    /**
     * @brief Генерирует текст тренировки.
     * @param goal Цель (масса/рельеф/похудение).
     * @param sleep Часы сна.
     * @param cals Калории.
     * @param meal Часы после еды.
     * @param date Дата.
     * @param isAuto Флаг авто-режима.
     * @param manualGroups Список групп для ручного.
     * @return QString Текст программы.
     */
    static QString generateDynamicPlan(int goal, int sleep, int cals, int meal, const QDate& date, bool isAuto, const QStringList& manualGroups = {});
};