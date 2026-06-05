/**
 * @file main.cpp
 * @brief Точка входа в приложение CalorieTracker & Workout Planner.
 * @details Инициализирует экземпляр QApplication, создает главное окно и запускает 
 * цикл обработки событий. Содержит глобальный обработчик исключений.
 */
/**
 * @mainpage CalorieTracker & Workout Planner
 * * @section intro_sec Описание проекта
 * Данное приложение предназначено для отслеживания калорий и автоматического/ручного 
 * планирования тренировок. Система учитывает физическое состояние пользователя 
 * (сон, питание) и адаптирует нагрузку.
 * * @section features_sec Основные возможности
 * - Расчет суточной нормы калорий.
 * - Интерактивная карта мышц для выбора зон тренировки.
 * - Генерация динамических планов занятий.
 * - Ведение истории и отображение прогресса.
 */
#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"
#include <stdexcept>

/**
 * @brief Главная функция приложения
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов командной строки.
 * @return Код возврата.
 * @details Обернута в глобальный try-catch для обработки непредвиденных ошибок.
 */
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    try {
        MainWindow w;
        w.show();
        return a.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Критическая ошибка", QString("Приложение закрыто из-за ошибки:\n") + e.what());
        return 1;
    } catch (...) {
        QMessageBox::critical(nullptr, "Неизвестная ошибка", "Произошла фатальная ошибка.");
        return 1;
    }
}