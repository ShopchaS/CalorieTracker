/**
 * @file MainWindow.cpp
 * @brief Реализация логики главного окна и взаимодействия с пользователем.
 * @details Содержит код настройки UI, логику переключения профилей, 
 * алгоритмы сохранения планов и обработку исключений при вводе данных.
 */
#include "MainWindow.h"
#include "WorkoutCalculator.h"
#include "MuscleMapWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QDialog>
#include <QTextEdit>
#include <QInputDialog>
#include <QAbstractSpinBox>
#include <QBitmap>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("CalorieTracker & Workout Planner");
    resize(450, 750);
    try {
        db.load();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Загрузка", e.what());
    }
    setupUI();
    loadProfilesIntoUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    setupProfileTab();
    setupWorkoutsTab();
    setupHistoryTab();
    
    tabWidget->setTabVisible(1, false);
    tabWidget->setTabVisible(2, false);

    connect(profileSelector, &QComboBox::currentTextChanged, this, &MainWindow::onProfileSelected);
    connect(newProfileBtn, &QPushButton::clicked, this, &MainWindow::onCreateNewProfile);
    connect(deleteProfileBtn, &QPushButton::clicked, this, &MainWindow::onDeleteProfile);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditProfile);
    connect(calculateBtn, &QPushButton::clicked, this, &MainWindow::onCalculateAndSave);
    connect(autoPlanBtn, &QPushButton::clicked, this, &MainWindow::onAutoPlanClicked);
    connect(manualPlanBtn, &QPushButton::clicked, this, &MainWindow::onManualPlanClicked);
    connect(goalCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onGoalChanged);
    connect(generateAutoPlanBtn, &QPushButton::clicked, this, &MainWindow::onGenerateAutoPlan);
    connect(workoutCalendar, &QCalendarWidget::clicked, this, &MainWindow::onCalendarClicked);
}

void MainWindow::setupProfileTab() {
    profileTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(profileTab);
    QHBoxLayout *top = new QHBoxLayout();
    
    top->addWidget(new QLabel("Профиль:"));
    profileSelector = new QComboBox();
    top->addWidget(profileSelector, 1);
    
    newProfileBtn = new QPushButton("Создать");
    deleteProfileBtn = new QPushButton("Удалить");
    deleteProfileBtn->setStyleSheet("color: #e74c3c; font-weight: bold;");
    
    top->addWidget(newProfileBtn);
    top->addWidget(deleteProfileBtn);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #dcdde1;");

    profileStack = new QStackedWidget();
    QWidget *welcome = new QWidget();
    QVBoxLayout *wl = new QVBoxLayout(welcome);
    wl->addWidget(new QLabel("Выберите профиль или создайте новый"), 0, Qt::AlignCenter);
    profileStack->addWidget(welcome);

    QWidget *form = new QWidget();
    QFormLayout *fl = new QFormLayout(form);
    nameInput = new QLineEdit();
    ageInput = new QSpinBox(); ageInput->setRange(10,120);
    weightInput = new QDoubleSpinBox(); weightInput->setRange(30,300); weightInput->setSuffix(" кг");
    heightInput = new QDoubleSpinBox(); heightInput->setRange(100,250); heightInput->setSuffix(" см");
    genderInput = new QComboBox(); genderInput->addItems({"Мужской", "Женский"});
    activityInput = new QComboBox(); activityInput->addItems({
        "Сидячий (1.2)", "Легкая активность (1.375)", "Средняя активность (1.55)", 
        "Высокая активность (1.725)", "Экстремальная (1.9)"});
    
    fl->addRow("Имя:", nameInput);
    fl->addRow("Возраст:", ageInput);
    fl->addRow("Вес:", weightInput);
    fl->addRow("Рост:", heightInput);
    fl->addRow("Пол:", genderInput);
    fl->addRow("Активность:", activityInput);
    
    editBtn = new QPushButton("Редактировать");
    calculateBtn = new QPushButton("Рассчитать и сохранить");
    calculateBtn->setStyleSheet("background-color: #4CAF50; color: white; font-weight: bold; padding: 8px;");
    
    QHBoxLayout *bl = new QHBoxLayout();
    bl->addWidget(editBtn); bl->addWidget(calculateBtn);
    fl->addRow(bl);
    resultLabel = new QLabel(); 
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    fl->addRow(resultLabel);
    profileStack->addWidget(form);

    imageLabel = new QLabel();
    QPixmap px(":/logo.png");
    if(!px.isNull()) {
        px = px.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        px.setMask(px.createMaskFromColor(Qt::white)); 
        imageLabel->setPixmap(px);
    }

    layout->addLayout(top);
    layout->addWidget(line);
    layout->addWidget(profileStack);
    layout->addSpacing(10);
    layout->addWidget(imageLabel, 0, Qt::AlignCenter);
    layout->addStretch();
    tabWidget->addTab(profileTab, "Профиль");
}

void MainWindow::setupWorkoutsTab() {
    workoutsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(workoutsTab);
    workoutCalendar = new QCalendarWidget();
    workoutCalendar->setGridVisible(true);
    layout->addWidget(workoutCalendar);

    QHBoxLayout *ml = new QHBoxLayout();
    autoPlanBtn = new QPushButton("Сгенерировать план");
    manualPlanBtn = new QPushButton("Создать свой план");
    ml->addWidget(autoPlanBtn); ml->addWidget(manualPlanBtn);
    layout->addLayout(ml);

    autoPlanWidget = new QWidget();
    QVBoxLayout *al = new QVBoxLayout(autoPlanWidget);
    durationCombo = new QComboBox(); durationCombo->addItems({"1 месяц","6 месяцев","1 год"});
    goalCombo = new QComboBox(); goalCombo->addItems({"Набор массы","Поддержание формы","Похудение"});
    
    generateAutoPlanBtn = new QPushButton("Создать расписание");
    generateAutoPlanBtn->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
    
    cancelAutoPlanBtn = new QPushButton("Отмена");
    cancelAutoPlanBtn->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold;");
    
    goalImageLabel = new QLabel();
    goalImageLabel->setAlignment(Qt::AlignCenter);

    al->addWidget(new QLabel("Срок:")); al->addWidget(durationCombo);
    al->addWidget(new QLabel("Цель:")); al->addWidget(goalCombo);
    al->addWidget(goalImageLabel);
    al->addWidget(generateAutoPlanBtn);
    al->addWidget(cancelAutoPlanBtn);
    
    autoPlanWidget->setVisible(false);
    layout->addWidget(autoPlanWidget);

    manualPlanWidget = new QWidget();
    QVBoxLayout *manL = new QVBoxLayout(manualPlanWidget);
    manualHintLabel = new QLabel("Выберите даты в календаре для добавления");
    stopManualPlanBtn = new QPushButton("Прекратить добавление");
    stopManualPlanBtn->setStyleSheet("background-color: #e74c3c; color: white;");
    manL->addWidget(manualHintLabel); manL->addWidget(stopManualPlanBtn);
    manualPlanWidget->setVisible(false);
    layout->addWidget(manualPlanWidget);

    connect(stopManualPlanBtn, &QPushButton::clicked, [this](){ manualPlanWidget->setVisible(false); });
    connect(cancelAutoPlanBtn, &QPushButton::clicked, [this](){ autoPlanWidget->setVisible(false); });

    layout->addStretch();
    tabWidget->addTab(workoutsTab, "Тренировки");
}

void MainWindow::setupHistoryTab() {
    historyTab = new QWidget();
    QVBoxLayout *l = new QVBoxLayout(historyTab);
    
    historyProgressLabel = new QLabel("Общий прогресс: 0%");
    historyProgressLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    historyProgressBar = new QProgressBar();
    historyProgressBar->setRange(0, 100);
    historyProgressBar->setValue(0);
    historyProgressBar->setStyleSheet("QProgressBar { border: 1px solid #bdc3c7; border-radius: 5px; text-align: center; } QProgressBar::chunk { background-color: #2ecc71; }");
    
    l->addWidget(historyProgressLabel);
    l->addWidget(historyProgressBar);

    l->addWidget(new QLabel("История выполненных тренировок (2x клик для деталей):"));
    historyList = new QListWidget();
    
    connect(historyList, &QListWidget::itemDoubleClicked, [this](QListWidgetItem *item){
        int row = historyList->row(item);
        if (row >= 0 && row < db.current().history.size()) {
            QString date = QString::fromStdString(db.current().history[row].name);
            QString details = QString::fromStdString(db.current().history[row].details);
            QMessageBox::information(this, "Отчет за " + date, details);
        }
    });

    historyLogoLabel = new QLabel();
    QPixmap px(":/logo.png");
    if(!px.isNull()) {
        px = px.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        px.setMask(px.createMaskFromColor(Qt::white));
        historyLogoLabel->setPixmap(px);
    }

    l->addWidget(historyList);
    l->addWidget(historyLogoLabel, 0, Qt::AlignCenter);
    tabWidget->addTab(historyTab, "История");
}

void MainWindow::updateCalendarColors() {
    workoutCalendar->setDateTextFormat(QDate(), QTextCharFormat());
    QTextCharFormat green; 
    green.setBackground(QColor("#2ecc71")); 
    green.setForeground(Qt::white);
    
    for (auto const& [d, p] : plannedDates) {
        workoutCalendar->setDateTextFormat(d, green);
    }
}

void MainWindow::syncPlanToDB() {
    if (db.currentProfileName.empty()) return;
    db.current().plan.clear();
    for(auto const& [d, p] : plannedDates) {
        db.current().plan[d.toString(Qt::ISODate).toStdString()] = p;
    }
    db.save();
}

void MainWindow::loadPlanFromDB() {
    plannedDates.clear();
    if (!db.currentProfileName.empty()) {
        for(auto const& [dStr, p] : db.current().plan) {
            plannedDates[QDate::fromString(QString::fromStdString(dStr), Qt::ISODate)] = p;
        }
    }
    updateCalendarColors();
}

void MainWindow::onCalendarClicked(const QDate &date) {
    try {
        auto it = plannedDates.find(date);
        QTextCharFormat green; 
        green.setBackground(QColor("#2ecc71")); 
        green.setForeground(Qt::white);

        if (it != plannedDates.end()) {
            PlannedWorkout info = it->second;
            QDialog diag(this); diag.setWindowTitle("План на " + date.toString("dd.MM.yyyy"));
            QVBoxLayout *l = new QVBoxLayout(&diag);
            
            QFormLayout *f = new QFormLayout();
            QSpinBox *s = new QSpinBox(); s->setRange(0,24); s->setValue(8); s->setSuffix(" ч.");
            QSpinBox *c = new QSpinBox(); c->setRange(0,5000); c->setValue(2000); c->setSuffix(" ккал");
            QSpinBox *m = new QSpinBox(); m->setRange(0,24); m->setValue(2); m->setSuffix(" ч. назад");
            f->addRow("Сон:", s); f->addRow("Ккал:", c); f->addRow("Еда:", m);
            l->addLayout(f);

            QTextEdit *txt = new QTextEdit(); txt->setReadOnly(true); l->addWidget(txt);

            auto updateText = [=]() {
                txt->setText(WorkoutCalculator::generateDynamicPlan(info.autoGoal, s->value(), c->value(), m->value(), date, info.isAuto, info.manualGroups));
            };
            connect(s, QOverload<int>::of(&QSpinBox::valueChanged), updateText);
            connect(c, QOverload<int>::of(&QSpinBox::valueChanged), updateText);
            connect(m, QOverload<int>::of(&QSpinBox::valueChanged), updateText);
            updateText();
            
            QHBoxLayout *ctrlLayout = new QHBoxLayout();
            QPushButton *done = new QPushButton("Выполнено");
            done->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold;");
            
            QPushButton *delBtn = new QPushButton("Удалить");
            delBtn->setStyleSheet("background-color: #e74c3c; color: white;");

            QDate nextFree = date.addDays(1);
            while (plannedDates.count(nextFree)) nextFree = nextFree.addDays(1);
            
            QPushButton *moveBtn = new QPushButton("Перенести (" + nextFree.toString("dd.MM") + ")");
            moveBtn->setStyleSheet("background-color: #f1c40f; color: black;");

            ctrlLayout->addWidget(done);
            ctrlLayout->addWidget(moveBtn);
            ctrlLayout->addWidget(delBtn);
            l->addLayout(ctrlLayout);

            connect(done, &QPushButton::clicked, [&](){
                try {
                    db.current().history.push_back({date.toString("dd.MM.yyyy").toStdString(), txt->toPlainText().toStdString()});
                    plannedDates.erase(date);
                    syncPlanToDB();
                    updateCalendarColors();
                    updateHistoryUI(); 
                    diag.accept();
                } catch(const std::exception& e) {
                    QMessageBox::critical(&diag, "Ошибка", e.what());
                }
            });

            connect(delBtn, &QPushButton::clicked, [&](){
                plannedDates.erase(date);
                syncPlanToDB();
                updateCalendarColors();
                updateHistoryUI();
                diag.accept();
            });

            connect(moveBtn, &QPushButton::clicked, [&](){
                plannedDates[nextFree] = info;
                plannedDates.erase(date);
                syncPlanToDB();
                updateCalendarColors();
                diag.accept();
            });

            diag.exec();
        } else if (manualPlanWidget->isVisible()) {
            QDialog sel(this); sel.setWindowTitle("Выбор мышц");
            MuscleMapWidget *map = new MuscleMapWidget(&sel);
            QVBoxLayout *sl = new QVBoxLayout(&sel);
            sl->addWidget(map);
            QPushButton *ok = new QPushButton("Добавить в план");
            sl->addWidget(ok);
            connect(ok, &QPushButton::clicked, [&](){
                try {
                    QStringList selected = map->getSelectedGroups();
                    if (selected.isEmpty()) throw std::invalid_argument("Не выбрана ни одна мышца.");
                    plannedDates[date] = {false, 0, selected};
                    syncPlanToDB();
                    updateCalendarColors();
                    updateHistoryUI(); 
                    sel.accept();
                } catch(const std::exception& e) {
                    QMessageBox::warning(&sel, "Ошибка ввода", e.what());
                }
            });
            sel.exec();
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Сбой календаря", e.what());
    }
}

void MainWindow::onProfileSelected(const QString &n) {
    try {
        workoutCalendar->setDateTextFormat(QDate(), QTextCharFormat());
        plannedDates.clear();

        if(n=="--- Выберите профиль ---" || n.isEmpty()) { 
            profileStack->setCurrentIndex(0); 
            tabWidget->setTabVisible(1,false); 
            tabWidget->setTabVisible(2,false);
            return; 
        }

        db.currentProfileName = n.toStdString();
        User u = db.current().user;
        
        nameInput->setText(QString::fromStdString(u.name));
        ageInput->setValue(u.age); 
        weightInput->setValue(u.weight); 
        heightInput->setValue(u.height);
        genderInput->setCurrentIndex(u.gender == 'M' ? 0 : 1);
        resultLabel->setText("Норма калорий: " + QString::number(u.daily_calories) + " ккал");
        
        loadPlanFromDB(); 

        setFormEnabled(false); 
        editBtn->show(); 
        calculateBtn->hide();
        
        profileStack->setCurrentIndex(1); 
        tabWidget->setTabVisible(1,true); 
        tabWidget->setTabVisible(2,true);
        
        updateHistoryUI();
    } catch(const std::exception& e) {
        QMessageBox::critical(this, "Ошибка", e.what());
    }
}

void MainWindow::onCreateNewProfile() { 
    profileSelector->setCurrentIndex(0);
    profileStack->setCurrentIndex(1); 
    setFormEnabled(true); 
    nameInput->clear();
    editBtn->hide(); 
    calculateBtn->show(); 
}

void MainWindow::onDeleteProfile() {
    try {
        QString current = profileSelector->currentText();
        if (current == "--- Выберите профиль ---") throw std::runtime_error("Профиль не выбран.");

        auto reply = QMessageBox::question(this, "Удаление", "Удалить профиль " + current + "?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            db.profiles.erase(current.toStdString());
            db.save();
            loadProfilesIntoUI();
            onProfileSelected("");
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::onEditProfile() { setFormEnabled(true); editBtn->hide(); calculateBtn->show(); }

void MainWindow::onCalculateAndSave() {
    try {
        if(nameInput->text().trimmed().isEmpty()) throw std::invalid_argument("Имя не может быть пустым.");
        
        User u; 
        u.name = nameInput->text().toStdString(); 
        u.age = ageInput->value();
        u.weight = weightInput->value(); 
        u.height = heightInput->value();
        u.gender = (genderInput->currentIndex()==0)?'M':'F';
        u.activity_level = 1.2 + (activityInput->currentIndex() * 0.175); 
        u.daily_calories = WorkoutCalculator::calculateDailyCalories(u);
        
        db.profiles[u.name].user = u; 
        db.currentProfileName = u.name; 
        db.save();
        
        loadProfilesIntoUI();
        profileSelector->setCurrentText(QString::fromStdString(u.name));
    } catch(const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", e.what());
    }
}

void MainWindow::onAutoPlanClicked() { 
    autoPlanWidget->setVisible(true); 
    manualPlanWidget->setVisible(false); 
    onGoalChanged(goalCombo->currentIndex()); 
}

void MainWindow::onManualPlanClicked() { 
    autoPlanWidget->setVisible(false); 
    manualPlanWidget->setVisible(true); 
}

void MainWindow::onGoalChanged(int index) {
    QString f = (index == 0) ? ":/mass.png" : (index == 1 ? ":/maintain.png" : ":/lose.png");
    QPixmap px(f);
    if(!px.isNull()) {
        px = px.scaled(240, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        goalImageLabel->setPixmap(px);
    }
}

void MainWindow::onGenerateAutoPlan() {
    try {
        int g = goalCombo->currentIndex();
        int durationIndex = durationCombo->currentIndex();
        QDate d = QDate::currentDate();
        
        int daysToPlan = 30; 
        if (durationIndex == 1) {
            daysToPlan = 183; 
        } else if (durationIndex == 2) {
            daysToPlan = 365; 
        }
        
        for(int i = 0; i < daysToPlan; ++i) {
            QDate cur = d.addDays(i);
            if(cur.dayOfWeek() == 1 || cur.dayOfWeek() == 3 || cur.dayOfWeek() == 5) {
                plannedDates[cur] = {true, g, {}};
            }
        }
        syncPlanToDB();
        updateCalendarColors();
        updateHistoryUI(); 
        autoPlanWidget->setVisible(false);
    } catch(const std::exception& e) {
        QMessageBox::critical(this, "Сбой", e.what());
    }
}

void MainWindow::loadProfilesIntoUI() {
    profileSelector->blockSignals(true); 
    profileSelector->clear();
    profileSelector->addItem("--- Выберите профиль ---");
    for(auto const& [name, p] : db.profiles) profileSelector->addItem(QString::fromStdString(name));
    profileSelector->blockSignals(false);
}

void MainWindow::updateHistoryUI() {
    historyList->clear();
    if(!db.currentProfileName.empty()) {
        int completed = db.current().history.size();
        int planned = db.current().plan.size();
        int total = completed + planned;

        if (total > 0) {
            int percent = (completed * 100) / total;
            historyProgressBar->setValue(percent);
            historyProgressLabel->setText(QString("Общий прогресс: выполнено %1 из %2").arg(completed).arg(total));
        } else {
            historyProgressBar->setValue(0);
            historyProgressLabel->setText("Общий прогресс: нет заданных тренировок");
        }

        for(auto const& w : db.current().history) {
            historyList->addItem(QString::fromStdString(w.name));
        }
    } else {
        historyProgressBar->setValue(0);
        historyProgressLabel->setText("Общий прогресс: 0%");
    }
}

void MainWindow::setFormEnabled(bool e) {
    nameInput->setEnabled(e); 
    ageInput->setEnabled(e); 
    weightInput->setEnabled(e); 
    heightInput->setEnabled(e);
    genderInput->setEnabled(e); 
    activityInput->setEnabled(e);
    auto symbols = e ? QAbstractSpinBox::UpDownArrows : QAbstractSpinBox::NoButtons;
    ageInput->setButtonSymbols(symbols);
    weightInput->setButtonSymbols(symbols);
    heightInput->setButtonSymbols(symbols);
}