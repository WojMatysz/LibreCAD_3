#include "coordinategui.h"
#include "ui_coordinategui.h"

#include <QTimer>
#include <QDoubleValidator>

using namespace lc::ui::api;

CoordinateGUI::CoordinateGUI(std::string label, QWidget* parent)
    :
    InputGUI(label, parent),
    _pointSelectionEnabled(false),
    mainWindow(nullptr),
    ui(new Ui::CoordinateGUI)
{
    ui->setupUi(this);
    _type = "coordinate";

    _textLabel = qobject_cast<QLabel*>(ui->horizontalLayout->itemAt(0)->widget());
    _xcoordEdit = qobject_cast<QLineEdit*>(ui->horizontalLayout->itemAt(2)->widget());
    _ycoordEdit = qobject_cast<QLineEdit*>(ui->horizontalLayout->itemAt(4)->widget());
    _pointButton = qobject_cast<QPushButton*>(ui->horizontalLayout->itemAt(5)->widget());
    ui->horizontalLayout->insertStretch(1);
    _pointButton->setStyleSheet("padding-left: 1px; padding-right: 1px;");

    QDoubleValidator* doubleValidator = new QDoubleValidator(-1000000, 1000000, 4, this);
    doubleValidator->setNotation(QDoubleValidator::StandardNotation);

    QFontMetrics fm = _xcoordEdit->fontMetrics();
    _xcoordEdit->setFixedWidth(fm.averageCharWidth() * 17);
    _ycoordEdit->setFixedWidth(fm.averageCharWidth() * 17);

    _textLabel->setText(QString(this->label().c_str()));
    _xcoordEdit->setValidator(doubleValidator);
    _ycoordEdit->setValidator(doubleValidator);

    _xcoordEdit->setText(0);
    _ycoordEdit->setText(0);

    connect(_xcoordEdit, &QLineEdit::editingFinished, this, &CoordinateGUI::editingFinishedCallbacks);
    connect(_ycoordEdit, &QLineEdit::editingFinished, this, &CoordinateGUI::editingFinishedCallbacks);
    connect(_xcoordEdit, &QLineEdit::textChanged, this, &CoordinateGUI::textChangedCallbacks);
    connect(_ycoordEdit, &QLineEdit::textChanged, this, &CoordinateGUI::textChangedCallbacks);
    connect(_pointButton, &QPushButton::toggled, this, &CoordinateGUI::togglePointSelection);
}

CoordinateGUI::~CoordinateGUI() {
    delete ui;
}

void CoordinateGUI::updateCoordinate() {
    _coordinate = lc::geo::Coordinate(_xcoordEdit->text().toDouble(), _ycoordEdit->text().toDouble());
}

void CoordinateGUI::updateCoordinateDisplay() {
    this->setToolTip(generateTooltip());
}

QString CoordinateGUI::generateTooltip() const {
    QString xcoordText = _xcoordEdit->text();
    QString ycoordText = _ycoordEdit->text();
    if (xcoordText == "") {
        xcoordText = "0";
    }
    if (ycoordText == "") {
        ycoordText = "0";
    }
    QString tooltipcoord = QString("(%1,%2)").arg(xcoordText, ycoordText);
    QString tooltipmagnitude = QString("Magnitude - %1").arg(QString::number(_coordinate.magnitude()));
    QString tooltipangle = QString("Angle - %1").arg(QString::number(_coordinate.angle() * 180 / 3.141592));
    return tooltipcoord + "\n" + tooltipmagnitude + "\n" + tooltipangle;
}

void CoordinateGUI::addFinishCallback(sol::function cb) {
    _callbacks_finished.push_back(cb);
}

void CoordinateGUI::addOnChangeCallback(sol::function cb) {
    _callbacks_onchange.push_back(cb);
}

void CoordinateGUI::editingFinishedCallbacks() {
    updateCoordinate();

    for (const auto & callback : _callbacks_finished) {
        callback(_coordinate);
    }
}

void CoordinateGUI::textChangedCallbacks() {
    updateCoordinate();
    updateCoordinateDisplay();

    for (const auto & callback : _callbacks_onchange) {
        callback(_coordinate);
    }
}

void CoordinateGUI::setLabel(const std::string& newLabel) {
    InputGUI::setLabel(newLabel);
    _textLabel->setText(QString(newLabel.c_str()));
}

lc::geo::Coordinate CoordinateGUI::value() const {
    return _coordinate;
}

void CoordinateGUI::setValue(lc::geo::Coordinate coord) {
    // so that textChanged isn't emitted twice
    _xcoordEdit->blockSignals(true);
    _xcoordEdit->setText(QString::number(coord.x()));
    _ycoordEdit->setText(QString::number(coord.y()));
    _xcoordEdit->blockSignals(false);
}

void CoordinateGUI::getLuaValue(sol::table & table) {
    table[_key] = value();
}

void CoordinateGUI::enableCoordinateSelection(lc::ui::MainWindow* mainWindow) {
    if (this->mainWindow != nullptr) {
        return;
    }

    this->mainWindow = mainWindow;
    if (mainWindow != nullptr) {
        connect(mainWindow, &lc::ui::MainWindow::point, this, &CoordinateGUI::pointSelected);
    }
}

void CoordinateGUI::pointSelected(lc::geo::Coordinate point) {
    if (_pointSelectionEnabled) {
        _coordinate = point;
        this->setValue(point);
        parentWidget()->activateWindow();
        this->setFocus();
        _pointButton->toggled(false);
        _pointButton->setChecked(false);
        editingFinishedCallbacks();
    }
}

void CoordinateGUI::togglePointSelection(bool toggle) {
    _pointSelectionEnabled = toggle;
    if (toggle) {
        mainWindow->activateWindow();
    }
}

void CoordinateGUI::copyValue(QDataStream& stream) {
    lc::geo::Coordinate coord = value();
    stream << coord.x() << coord.y() << coord.z();
}

void CoordinateGUI::pasteValue(QDataStream& stream) {
    double x, y, z;
    stream >> x >> y >> z;
    setValue(lc::geo::Coordinate(x, y, z));
    editingFinishedCallbacks();
}

void CoordinateGUI::hideLabel() {
    _textLabel->hide();
}
