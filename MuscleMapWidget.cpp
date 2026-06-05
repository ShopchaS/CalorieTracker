/**
 * @file MuscleMapWidget.cpp
 * @brief Реализация отрисовки и хит-тестов для карты мышц.
 * @details Содержит координаты всех мышечных зон (QPainterPath) и логику 
 * переключения видов тела.
 */
#include "MuscleMapWidget.h"
#include <QPolygonF>
#include <QBoxLayout>
#include <QFont>
#include <QFormLayout> 
#include <stdexcept>

MuscleMapWidget::MuscleMapWidget(QWidget *parent) : QWidget(parent) {
    setFixedSize(400, 700); 
    
    if (!frontPixmap.load(":/body_front.png") || !backPixmap.load(":/body_back.png")) {
        throw std::runtime_error("Файлы изображений (body_front.png, body_back.png) не найдены!");
    }

    initZones();

    frontBtn = new QPushButton("Спереди", this);
    backBtn = new QPushButton("Сзади", this);
    
    frontBtn->setCheckable(true);
    backBtn->setCheckable(true);
    frontBtn->setChecked(true);

    QString btnStyle = "QPushButton { background-color: #f5f6fa; border: 2px solid #dcdde1; border-radius: 5px; padding: 5px; } "
                       "QPushButton:checked { background-color: #3498db; color: white; border: 2px solid #2980b9; }";
    frontBtn->setStyleSheet(btnStyle);
    backBtn->setStyleSheet(btnStyle);

    connect(frontBtn, &QPushButton::clicked, [this](){ 
        showFront = true; backBtn->setChecked(false); update(); 
    });
    connect(backBtn, &QPushButton::clicked, [this](){ 
        showFront = false; frontBtn->setChecked(false); update(); 
    });
}

void MuscleMapWidget::initZones() {
    QColor c_abs = QColor(241, 196, 15, 160);
    QColor c_chest = QColor(231, 76, 60, 160);
    QColor c_legsF = QColor(155, 89, 182, 160);
    QColor c_shoulders = QColor(52, 152, 219, 160);
    QColor c_armsF = QColor(26, 188, 156, 160);
    QColor c_head = QColor(127, 140, 141, 160);

    QPainterPath headF; 
    headF.addEllipse(QPointF(200, 45), 40, 50); 
    headF.addRect(180, 85, 40, 10); 
    zones["Шея"].path = headF; zones["Шея"].isFront = true; zones["Шея"].fillColor = c_head;

    QPainterPath shF;
    shF.moveTo(115, 105); shF.quadTo(70, 160, 120, 220); shF.lineTo(50, 220); shF.quadTo(40, 160, 115, 105); 
    shF.moveTo(285, 105); shF.quadTo(330, 160, 280, 220); shF.lineTo(350, 220); shF.quadTo(360, 160, 285, 105); 
    zones["Плечи"].path = shF; zones["Плечи"].isFront = true; zones["Плечи"].fillColor = c_shoulders;

    QPainterPath ch; 
    ch.moveTo(175, 90); 
    ch.lineTo(225, 90); 
    ch.lineTo(285, 105); 
    ch.quadTo(330, 160, 280, 220); 
    ch.quadTo(200, 250, 120, 220); 
    ch.quadTo(70, 160, 115, 105);  
    ch.closeSubpath();
    zones["Грудь"].path = ch; zones["Грудь"].isFront = true; zones["Грудь"].fillColor = c_chest;

    QPainterPath abs; abs.moveTo(120, 220); abs.quadTo(200, 250, 280, 220); abs.lineTo(280, 380); abs.lineTo(120, 380); abs.closeSubpath();
    zones["Пресс"].path = abs; zones["Пресс"].isFront = true; zones["Пресс"].fillColor = c_abs;

    QPainterPath armsF; armsF.addRect(50, 220, 70, 100); armsF.addRect(280, 220, 70, 100);
    zones["Руки"].path = armsF; zones["Руки"].isFront = true; zones["Руки"].fillColor = c_armsF;

    QPainterPath legsF;
    legsF.addPolygon(QPolygonF({QPointF(120, 380), QPointF(120, 500), QPointF(180, 500), QPointF(190, 380)}));
    legsF.addPolygon(QPolygonF({QPointF(280, 380), QPointF(280, 500), QPointF(220, 500), QPointF(210, 380)}));
    zones["Квадрицепс"].path = legsF; zones["Квадрицепс"].isFront = true; zones["Квадрицепс"].fillColor = c_legsF;

    QColor c_backGen = QColor(41, 128, 185, 160);
    QColor c_glutes = QColor(230, 126, 34, 160); 
    QColor c_ham = QColor(142, 68, 173, 160);    
    QColor c_calves = QColor(39, 174, 96, 160);  
    QColor c_tris = QColor(22, 160, 133, 160);   

    QPainterPath backGen;
    backGen.moveTo(175, 90);
    backGen.lineTo(225, 90);
    backGen.lineTo(285, 105);
    backGen.lineTo(280, 220); 
    backGen.lineTo(280, 380); 
    backGen.lineTo(120, 380); 
    backGen.lineTo(120, 220); 
    backGen.lineTo(115, 105); 
    backGen.closeSubpath();
    zones["Спина"].path = backGen; zones["Спина"].isFront = false; zones["Спина"].fillColor = c_backGen;

    QPainterPath tr; QPainterPath shB;
    shB.moveTo(115, 105); shB.quadTo(60, 150, 100, 220); shB.lineTo(120, 220); shB.closeSubpath(); 
    shB.moveTo(285, 105); shB.quadTo(340, 150, 300, 220); shB.lineTo(280, 220); shB.closeSubpath(); 
    tr.addRect(50, 220, 70, 100); tr.addRect(280, 220, 70, 100); 
    tr.addPath(shB); 
    zones["Трицепс Плечи (Зад)"].path = tr; zones["Трицепс Плечи (Зад)"].isFront = false; zones["Трицепс Плечи (Зад)"].fillColor = c_tris;

    QPainterPath gl; 
    gl.moveTo(120, 380); gl.quadTo(120, 420, 150, 440); gl.cubicTo(170, 450, 190, 450, 200, 450); gl.lineTo(200, 380); gl.closeSubpath(); 
    gl.moveTo(280, 380); gl.quadTo(280, 420, 250, 440); gl.cubicTo(230, 450, 210, 450, 200, 450); gl.lineTo(200, 380); gl.closeSubpath(); 
    zones["Ягодицы"].path = gl; zones["Ягодицы"].isFront = false; zones["Ягодицы"].fillColor = c_glutes;

    QPainterPath ham;
    ham.moveTo(130, 440); ham.cubicTo(110, 450, 110, 490, 130, 500); ham.lineTo(190, 500); ham.lineTo(200, 450); ham.closeSubpath(); 
    ham.moveTo(270, 440); ham.cubicTo(290, 450, 290, 490, 270, 500); ham.lineTo(210, 500); ham.lineTo(200, 450); ham.closeSubpath(); 
    zones["Задняя пов. бедра"].path = ham; zones["Задняя пов. бедра"].isFront = false; zones["Задняя пов. бедра"].fillColor = c_ham;

    QPainterPath calves;
    calves.moveTo(130, 500); calves.cubicTo(110, 500, 110, 620, 130, 620); calves.lineTo(180, 620); calves.lineTo(190, 500); calves.closeSubpath(); 
    calves.moveTo(270, 500); calves.cubicTo(290, 500, 290, 620, 270, 620); calves.lineTo(220, 620); calves.lineTo(210, 500); calves.closeSubpath(); 
    zones["Икры"].path = calves; zones["Икры"].isFront = false; zones["Икры"].fillColor = c_calves;
}

void MuscleMapWidget::resizeEvent(QResizeEvent *) {
    frontBtn->setGeometry(10, 10, 185, 40);
    backBtn->setGeometry(205, 10, 185, 40);
}

void MuscleMapWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QPixmap current = showFront ? frontPixmap : backPixmap;
    if (!current.isNull()) {
        painter.drawPixmap(rect(), current.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    for (auto it = zones.begin(); it != zones.end(); ++it) {
        if (it.value().isFront == showFront && it.value().selected) {
            painter.fillPath(it.value().path, it.value().fillColor);
            painter.setPen(QPen(QColor(39, 174, 96), 2));
            painter.drawPath(it.value().path);
        }
    }

    QStringList selectedNames = getSelectedGroups();
    if (!selectedNames.isEmpty()) {
        int yOffset = height() - 20; 
        
        painter.setFont(QFont("Arial", 14, QFont::Bold));
        for (int i = selectedNames.size() - 1; i >= 0; --i) {
            QString name = selectedNames[i];
            QColor c = zones[name].fillColor;
            c.setAlpha(255); 
            painter.setPen(c);
            painter.drawText(QRect(0, yOffset - 25, width() - 15, 25), Qt::AlignRight | Qt::AlignVCenter, name);
            yOffset -= 25; 
        }
        
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(QRect(0, yOffset - 30, width() - 15, 30), Qt::AlignRight | Qt::AlignVCenter, "В ПЛАНЕ:");
    }
}

void MuscleMapWidget::mousePressEvent(QMouseEvent *event) {
    if (event->pos().y() < 60) return;

    for (auto it = zones.begin(); it != zones.end(); ++it) {
        if (it.value().isFront == showFront && it.value().path.contains(event->pos())) {
            it.value().selected = !it.value().selected;
            update();
            return;
        }
    }
}

QStringList MuscleMapWidget::getSelectedGroups() const {
    QStringList selected;
    for (auto it = zones.begin(); it != zones.end(); ++it) {
        if (it.value().selected) selected << it.key();
    }
    return selected;
}

void MuscleMapWidget::clearSelection() {
    for (auto it = zones.begin(); it != zones.end(); ++it) it.value().selected = false;
    update();
}