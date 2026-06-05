/**
 * @file CoreData.h
 * @brief Определение структур данных и менеджера профилей.
 * @details Содержит структуры User, Workout, PlannedWorkout, Profile, а также 
 * класс DataManager для работы с JSON-хранилищем.
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdexcept>

#include <QStringList>
#include <QDate>
#include <QCoreApplication>
#include <QDir>

// --- НАЧАЛО ЗАИМСТВОВАННОГО КОДА (Библиотека nlohmann/json) ---
#include "json.hpp"
using json = nlohmann::json;

namespace nlohmann {
    template <>
    struct adl_serializer<QStringList> {
        static void to_json(json& j, const QStringList& list) {
            std::vector<std::string> v;
            for (const auto& s : list) {
                v.push_back(s.toStdString());
            }
            j = v;
        }

        static void from_json(const json& j, QStringList& list) {
            list.clear();
            for (const std::string& s : j.get<std::vector<std::string>>()) {
                list.append(QString::fromStdString(s));
            }
        }
    };
}
// --- КОНЕЦ ЗАИМСТВОВАННОГО КОДА ---

/** @brief Структура данных пользователя */
struct User {
    std::string name = "Гость";  /**< Имя пользователя */
    int age = 25;                /**< Возраст */
    double weight = 70.0;        /**< Вес в кг */
    double height = 175.0;       /**< Рост в см */
    char gender = 'M';           /**< Пол ('M'/'F') */
    double activity_level = 1.2; /**< Коэффициент активности */
    int daily_calories = 0;      /**< Норма калорий */
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    User,
    name,
    age,
    weight,
    height,
    gender,
    activity_level,
    daily_calories
)

/** @brief Структура выполненной тренировки */
struct Workout {
    std::string name;    /**< Дата тренировки */
    std::string details; /**< Текст отчета о тренировке */
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Workout, name, details)

/** @brief Структура запланированной тренировки */
struct PlannedWorkout {
    bool isAuto = true;       /**< Создана ли автоматически */
    int autoGoal = 1;         /**< Цель для авто-режима */
    QStringList manualGroups; /**< Выбранные мышцы для ручного режима */
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    PlannedWorkout,
    isAuto,
    autoGoal,
    manualGroups
)

/** @brief Профиль пользователя со всеми данными */
struct Profile {
    User user;                                  /**< Данные пользователя */
    std::vector<Workout> history;               /**< История тренировок */
    std::map<std::string, PlannedWorkout> plan; /**< Календарный план */
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Profile, user, history, plan)

/**
 * @brief Менеджер сохранения и загрузки данных JSON.
 */
class DataManager {
private:
    /**
     * @brief Путь к JSON-файлу с данными.
     * @details Файл сохраняется рядом с запускаемым приложением.
     */
    std::string filename = QDir(QCoreApplication::applicationDirPath())
                               .filePath("tracker_data.json")
                               .toStdString();

public:
    std::map<std::string, Profile> profiles; /**< Все профили */
    std::string currentProfileName = "";     /**< Имя активного профиля */

    /**
     * @brief Сохраняет данные в JSON-файл.
     * @throw std::runtime_error При ошибке записи.
     */
    void save() {
        std::ofstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error("Не удалось открыть файл для записи.");
        }

        json j = profiles;
        file << j.dump(4);
        file.close();
    }

    /**
     * @brief Загружает данные из JSON-файла.
     * @throw std::runtime_error При повреждении файла.
     */
    void load() {
        std::ifstream file(filename);

        if (!file.is_open()) {
            return;
        }

        try {
            json j;
            file >> j;
            profiles = j.get<std::map<std::string, Profile>>();
        } catch (...) {
            throw std::runtime_error("Файл данных поврежден.");
        }

        file.close();
    }

    /**
     * @brief Возвращает текущий профиль.
     * @return Ссылка на текущий профиль.
     * @throw std::runtime_error Если профиль не выбран.
     */
    Profile& current() {
        if (
            currentProfileName.empty() ||
            profiles.find(currentProfileName) == profiles.end()
        ) {
            throw std::runtime_error("Профиль не выбран.");
        }

        return profiles[currentProfileName];
    }
};