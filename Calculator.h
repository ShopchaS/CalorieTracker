/**
 * @file Calculator.h
 * @brief Вспомогательные математические функции.
 * @details Содержит базовую формулу Миффлина-Сан Жеора для быстрой проверки 
 * корректности расчетов калорий.
 */
#pragma once
#include "CoreData.h"
#include <vector>

/**
 * @brief Вспомогательный класс для расчетов.
 */
class Calculator {
public:
    /**
     * @brief Формула Миффлина-Сан Жеора для расчета суточной нормы калорий.
     * @param user Объект пользователя.
     * @return int Суточная норма калорий.
     */
    static int calculateDailyCalories(const User& user) {
        double bmr = (10.0 * user.weight) + (6.25 * user.height) - (5.0 * user.age);
        if (user.gender == 'M') {
            bmr += 5;
        } else {
            bmr -= 161;
        }
        return static_cast<int>(bmr * user.activity_level);
    }
};