/**
 * @file MainWindow.h
 * @brief Заголовочный файл главного окна приложения.
 * @details Описывает интерфейс пользователя, слоты для обработки нажатий кнопок, 
 * управления календарем и навигации по вкладкам.
 */
#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QComboBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QCalendarWidget>
#include <QListWidget>
#include <QProgressBar>
#include <map>
#include "CoreData.h"
#include "WorkoutCalculator.h"
#include "MuscleMapWidget.h"

/**
 * @brief Класс главного окна приложения CalorieTracker.
 * @details Управляет графическим интерфейсом, переключением вкладок, профилями пользователей и календарем тренировок.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна.
     * @param parent Указатель на родительский виджет (по умолчанию nullptr).
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Деструктор главного окна.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Обработчик выбора профиля из выпадающего списка.
     * @param n Имя выбранного профиля.
     */
    void onProfileSelected(const QString &n);
    
    /** @brief Обработчик нажатия на кнопку создания нового профиля. */
    void onCreateNewProfile();
    
    /** @brief Обработчик удаления текущего профиля. */
    void onDeleteProfile();
    
    /** @brief Обработчик перехода в режим редактирования анкеты профиля. */
    void onEditProfile();
    
    /** @brief Обработчик сохранения данных профиля и расчета калорий. */
    void onCalculateAndSave();
    
    /** @brief Обработчик перехода в меню автоматического планирования. */
    void onAutoPlanClicked();
    
    /** @brief Обработчик перехода в меню ручного планирования. */
    void onManualPlanClicked();
    
    /**
     * @brief Обработчик смены цели тренировок (меняет картинку-подсказку).
     * @param index Индекс выбранной цели в QComboBox.
     */
    void onGoalChanged(int index);
    
    /** @brief Обработчик генерации автоматического плана на месяц. */
    void onGenerateAutoPlan();
    
    /**
     * @brief Обработчик клика по дате в календаре.
     * @param date Выбранная пользователем дата.
     */
    void onCalendarClicked(const QDate &date);

private:
    /** @brief Инициализация всех элементов UI и вкладок. */
    void setupUI();
    
    /** @brief Настройка интерфейса вкладки "Профиль". */
    void setupProfileTab();
    
    /** @brief Настройка интерфейса вкладки "Тренировки" (Календарь). */
    void setupWorkoutsTab();
    
    /** @brief Настройка интерфейса вкладки "История". */
    void setupHistoryTab();
    
    /** @brief Загрузка списка профилей из базы данных в выпадающий список UI. */
    void loadProfilesIntoUI();
    
    /** @brief Обновление списка выполненных тренировок и прогресс-бара. */
    void updateHistoryUI();
    
    /**
     * @brief Блокировка или разблокировка полей ввода профиля.
     * @param e Состояние активности (true - разблокировано, false - заблокировано).
     */
    void setFormEnabled(bool e);
    
    /** @brief Обновление цветов ячеек в календаре (подсветка тренировок). */
    void updateCalendarColors();
    
    /** @brief Синхронизация текущего локального плана с базой данных (JSON). */
    void syncPlanToDB();
    
    /** @brief Загрузка плана тренировок из базы данных в локальный кэш. */
    void loadPlanFromDB();

    DataManager db;                               /**< Менеджер данных, отвечающий за работу с JSON. */
    std::map<QDate, PlannedWorkout> plannedDates; /**< Локальный кэш запланированных тренировок для активного профиля. */

    QTabWidget *tabWidget;                        /**< Главный виджет вкладок. */
    
    QWidget *profileTab, *workoutsTab, *historyTab, *autoPlanWidget, *manualPlanWidget; 
    
    QComboBox *profileSelector, *durationCombo, *goalCombo, *genderInput, *activityInput;
    QPushButton *newProfileBtn, *deleteProfileBtn, *editBtn, *calculateBtn;
    QPushButton *autoPlanBtn, *manualPlanBtn, *generateAutoPlanBtn, *cancelAutoPlanBtn, *stopManualPlanBtn;
    QStackedWidget *profileStack;
    QLineEdit *nameInput;
    QSpinBox *ageInput;
    QDoubleSpinBox *weightInput, *heightInput;
    QLabel *resultLabel, *imageLabel, *goalImageLabel, *manualHintLabel, *historyLogoLabel;
    QCalendarWidget *workoutCalendar;
    QListWidget *historyList;
    QProgressBar *historyProgressBar;
    QLabel *historyProgressLabel;
};