/**
 * @file tests.cpp
 * @brief Модульные тесты для проверки ядра приложения.
 * @details Тесты оформлены с помощью Doctest и запускаются через CTest.
 * Проверяются расчет калорий, генерация тренировочных планов,
 * работа с профилями, историями, планами и JSON-сохранением.
 */
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "CoreData.h"
#include "WorkoutCalculator.h"
#include <QCoreApplication>
#include <QDate>
#include <QStringList>
#include <QDir>
#include <QFile>

static QString getDataFilePath() {
    return QDir(QCoreApplication::applicationDirPath()).filePath("tracker_data.json");
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    QString dataFilePath = getDataFilePath();
    QString backupFilePath = dataFilePath + ".test_backup";

    bool hadOriginalFile = QFile::exists(dataFilePath);

    if (QFile::exists(backupFilePath)) {
        QFile::remove(backupFilePath);
    }

    if (hadOriginalFile) {
        QFile::copy(dataFilePath, backupFilePath);
    }

    QFile::remove(dataFilePath);

    doctest::Context context;
    context.applyCommandLine(argc, argv);

    int result = context.run();

    QFile::remove(dataFilePath);

    if (hadOriginalFile) {
        QFile::copy(backupFilePath, dataFilePath);
        QFile::remove(backupFilePath);
    }

    if (context.shouldExit()) {
        return result;
    }

    return result;
}

TEST_CASE("CalculateCalories_Male") {
    User u{"Иван", 25, 80.0, 180.0, 'M', 1.55, 0};
    CHECK(WorkoutCalculator::calculateDailyCalories(u) == 2797);
}

TEST_CASE("CalculateCalories_Female") {
    User u{"Анна", 30, 60.0, 165.0, 'F', 1.2, 0};
    CHECK(WorkoutCalculator::calculateDailyCalories(u) == 1584);
}

TEST_CASE("CalculateCalories_HighActivity") {
    User u{"Спортсмен", 22, 90.0, 190.0, 'M', 1.9, 0};
    CHECK(WorkoutCalculator::calculateDailyCalories(u) > 3500);
}

TEST_CASE("GeneratePlan_Positive_IdealConditions") {
    QDate date(2026, 6, 1);
    QString plan = WorkoutCalculator::generateDynamicPlan(0, 8, 2500, 2, date, true);

    CHECK(plan.contains("Сон отличный"));
    CHECK(plan.contains("Энергии достаточно"));
    CHECK(plan.contains("4x8"));
    CHECK(plan.contains("ПРОГРАММА"));
}

TEST_CASE("GeneratePlan_Negative_CriticalCondition") {
    QDate date(2026, 6, 1);
    QString plan = WorkoutCalculator::generateDynamicPlan(0, 4, 800, 0, date, true);

    CHECK(plan.contains("КРИТИЧЕСКИЙ УРОВЕНЬ"));
    CHECK(plan.contains("Рекомендуется полный отдых"));
    CHECK_FALSE(plan.contains("ПРОГРАММА"));
}

TEST_CASE("GeneratePlan_AfterMeal_AbsRestriction") {
    QDate date(2026, 6, 3);
    QStringList manual = {"Пресс", "Руки"};

    QString plan = WorkoutCalculator::generateDynamicPlan(1, 8, 2000, 0, date, false, manual);

    CHECK(plan.contains("Исключены упражнения на пресс"));
    CHECK_FALSE(plan.contains("[Пресс]"));
    CHECK(plan.contains("[Руки]"));
}

TEST_CASE("GeneratePlan_CalorieDeficit") {
    QDate date(2026, 6, 1);
    QString plan = WorkoutCalculator::generateDynamicPlan(2, 7, 1400, 6, date, true);

    CHECK(plan.contains("Прошло много времени после еды"));
    CHECK(plan.contains("Дефицит калорий. Тренировка сокращена"));
}

TEST_CASE("DataManager_ExceptionOnEmptyProfile") {
    DataManager db;
    db.currentProfileName = "";

    CHECK_THROWS_AS(db.current(), std::runtime_error);
}

TEST_CASE("DataManager_ValidProfileAccess") {
    DataManager db;
    db.profiles["TestUser"].user.age = 28;
    db.currentProfileName = "TestUser";

    CHECK_NOTHROW(db.current());
    CHECK(db.current().user.age == 28);
}

TEST_CASE("DataManager_DeleteProfile") {
    DataManager db;
    db.profiles["DeleteMe"].user.name = "DeleteMe";

    CHECK(db.profiles.count("DeleteMe") == 1);

    db.profiles.erase("DeleteMe");

    CHECK(db.profiles.count("DeleteMe") == 0);
}

TEST_CASE("DataManager_SaveJSON") {
    DataManager db;
    db.profiles["TestSave"].user.name = "TestSave";
    db.profiles["TestSave"].plan["2026-05-10"] = PlannedWorkout{true, 1, {}};

    CHECK_NOTHROW(db.save());
    CHECK(QFile::exists(getDataFilePath()));
}

TEST_CASE("DataManager_LoadJSON") {
    DataManager db;
    db.profiles["LoadUser"].user.name = "LoadUser";
    db.profiles["LoadUser"].user.age = 19;

    CHECK_NOTHROW(db.save());

    DataManager db2;

    CHECK_NOTHROW(db2.load());
    CHECK(db2.profiles.count("LoadUser") == 1);
    CHECK(db2.profiles["LoadUser"].user.age == 19);
}

TEST_CASE("GeneratePlan_AutoPlan_MondayGroups") {
    QDate monday(2026, 6, 1);

    QString plan = WorkoutCalculator::generateDynamicPlan(0, 8, 2500, 2, monday, true);

    CHECK(plan.contains("[Грудь]"));
    CHECK(plan.contains("[Плечи]"));
    CHECK(plan.contains("[Трицепс Плечи (Зад)]"));

    CHECK_FALSE(plan.contains("[Квадрицепс]"));
    CHECK_FALSE(plan.contains("[Ягодицы]"));
}

TEST_CASE("GeneratePlan_AutoPlan_WednesdayGroups") {
    QDate wednesday(2026, 6, 3);

    QString plan = WorkoutCalculator::generateDynamicPlan(1, 8, 2200, 2, wednesday, true);

    CHECK(plan.contains("[Спина]"));
    CHECK(plan.contains("[Руки]"));
    CHECK(plan.contains("[Пресс]"));

    CHECK_FALSE(plan.contains("[Грудь]"));
    CHECK_FALSE(plan.contains("[Ягодицы]"));
}

TEST_CASE("GeneratePlan_AutoPlan_FridayGroups") {
    QDate friday(2026, 6, 5);

    QString plan = WorkoutCalculator::generateDynamicPlan(2, 8, 2200, 2, friday, true);

    CHECK(plan.contains("[Квадрицепс]"));
    CHECK(plan.contains("[Задняя пов. бедра]"));
    CHECK(plan.contains("[Ягодицы]"));
    CHECK(plan.contains("[Икры]"));

    CHECK_FALSE(plan.contains("[Грудь]"));
    CHECK_FALSE(plan.contains("[Пресс]"));
}

TEST_CASE("GeneratePlan_SleepDeficit_ReducesExerciseCount") {
    QDate date(2026, 6, 1);
    QStringList manual = {"Грудь"};

    QString plan = WorkoutCalculator::generateDynamicPlan(1, 6, 2200, 2, date, false, manual);

    CHECK(plan.contains("Недосып"));
    CHECK(plan.contains("[Грудь]"));
    CHECK(plan.count("💪") == 2);
}

TEST_CASE("GeneratePlan_NormalSleep_ThreeExercisesPerGroup") {
    QDate date(2026, 6, 1);
    QStringList manual = {"Грудь"};

    QString plan = WorkoutCalculator::generateDynamicPlan(1, 8, 2200, 2, date, false, manual);

    CHECK(plan.contains("Сон отличный"));
    CHECK(plan.contains("[Грудь]"));
    CHECK(plan.count("💪") == 3);
}

TEST_CASE("GeneratePlan_LowCalories_ReducesSets") {
    QDate date(2026, 6, 1);
    QStringList manual = {"Грудь"};

    QString plan = WorkoutCalculator::generateDynamicPlan(0, 8, 1600, 2, date, false, manual);

    CHECK(plan.contains("Средний уровень энергии"));
    CHECK(plan.contains("[Грудь]"));
    CHECK(plan.contains("3x8"));
    CHECK_FALSE(plan.contains("4x8"));
}

TEST_CASE("GeneratePlan_ManualGroups_MultipleMuscles") {
    QDate date(2026, 6, 2);
    QStringList manual = {"Грудь", "Руки", "Икры"};

    QString plan = WorkoutCalculator::generateDynamicPlan(1, 8, 2200, 2, date, false, manual);

    CHECK(plan.contains("[Грудь]"));
    CHECK(plan.contains("[Руки]"));
    CHECK(plan.contains("[Икры]"));

    CHECK_FALSE(plan.contains("[Спина]"));
    CHECK_FALSE(plan.contains("[Ягодицы]"));
}

TEST_CASE("DataManager_SaveLoad_ProfileWithHistoryAndPlan") {
    DataManager db;

    Profile profile;
    profile.user.name = "ComplexUser";
    profile.user.age = 21;
    profile.user.weight = 70.0;
    profile.user.height = 175.0;
    profile.user.gender = 'M';
    profile.user.activity_level = 1.55;
    profile.user.daily_calories = 2500;

    profile.history.push_back({"01.06.2026", "Тренировка груди"});
    profile.plan["2026-06-03"] = PlannedWorkout{true, 1, {}};

    db.profiles["ComplexUser"] = profile;

    CHECK_NOTHROW(db.save());

    DataManager db2;

    CHECK_NOTHROW(db2.load());
    CHECK(db2.profiles.count("ComplexUser") == 1);
    CHECK(db2.profiles["ComplexUser"].user.age == 21);
    CHECK(db2.profiles["ComplexUser"].history.size() == 1);
    CHECK(db2.profiles["ComplexUser"].plan.count("2026-06-03") == 1);
}
