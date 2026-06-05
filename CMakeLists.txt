cmake_minimum_required(VERSION 3.16)
project(CalorieTracker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(CalorieTracker
    main.cpp
    MainWindow.cpp
    WorkoutCalculator.cpp
    MuscleMapWidget.cpp
    MainWindow.h
    CoreData.h
    WorkoutCalculator.h
    MuscleMapWidget.h
    Calculator.h
    resources.qrc
)

target_link_libraries(CalorieTracker PRIVATE Qt6::Widgets)

enable_testing()

add_executable(RunTests
    tests.cpp
    WorkoutCalculator.cpp
    CoreData.h
    WorkoutCalculator.h
    Calculator.h
)

target_link_libraries(RunTests PRIVATE Qt6::Widgets)

add_test(NAME CalculateCalories_Male COMMAND RunTests --test-case=CalculateCalories_Male)
add_test(NAME CalculateCalories_Female COMMAND RunTests --test-case=CalculateCalories_Female)
add_test(NAME CalculateCalories_HighActivity COMMAND RunTests --test-case=CalculateCalories_HighActivity)

add_test(NAME GeneratePlan_Positive_IdealConditions COMMAND RunTests --test-case=GeneratePlan_Positive_IdealConditions)
add_test(NAME GeneratePlan_Negative_CriticalCondition COMMAND RunTests --test-case=GeneratePlan_Negative_CriticalCondition)
add_test(NAME GeneratePlan_AfterMeal_AbsRestriction COMMAND RunTests --test-case=GeneratePlan_AfterMeal_AbsRestriction)
add_test(NAME GeneratePlan_CalorieDeficit COMMAND RunTests --test-case=GeneratePlan_CalorieDeficit)

add_test(NAME DataManager_ExceptionOnEmptyProfile COMMAND RunTests --test-case=DataManager_ExceptionOnEmptyProfile)
add_test(NAME DataManager_ValidProfileAccess COMMAND RunTests --test-case=DataManager_ValidProfileAccess)
add_test(NAME DataManager_DeleteProfile COMMAND RunTests --test-case=DataManager_DeleteProfile)
add_test(NAME DataManager_SaveJSON COMMAND RunTests --test-case=DataManager_SaveJSON)
add_test(NAME DataManager_LoadJSON COMMAND RunTests --test-case=DataManager_LoadJSON)

add_test(NAME GeneratePlan_AutoPlan_MondayGroups COMMAND RunTests --test-case=GeneratePlan_AutoPlan_MondayGroups)
add_test(NAME GeneratePlan_AutoPlan_WednesdayGroups COMMAND RunTests --test-case=GeneratePlan_AutoPlan_WednesdayGroups)
add_test(NAME GeneratePlan_AutoPlan_FridayGroups COMMAND RunTests --test-case=GeneratePlan_AutoPlan_FridayGroups)

add_test(NAME GeneratePlan_SleepDeficit_ReducesExerciseCount COMMAND RunTests --test-case=GeneratePlan_SleepDeficit_ReducesExerciseCount)
add_test(NAME GeneratePlan_NormalSleep_ThreeExercisesPerGroup COMMAND RunTests --test-case=GeneratePlan_NormalSleep_ThreeExercisesPerGroup)
add_test(NAME GeneratePlan_LowCalories_ReducesSets COMMAND RunTests --test-case=GeneratePlan_LowCalories_ReducesSets)
add_test(NAME GeneratePlan_ManualGroups_MultipleMuscles COMMAND RunTests --test-case=GeneratePlan_ManualGroups_MultipleMuscles)

add_test(NAME DataManager_SaveLoad_ProfileWithHistoryAndPlan COMMAND RunTests --test-case=DataManager_SaveLoad_ProfileWithHistoryAndPlan)