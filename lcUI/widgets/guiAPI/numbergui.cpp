#include "numbergui.h"
#include "ui_numbergui.h"

using namespace lc::ui::api;

NumberGUI::NumberGUI(std::string label, double minVal, double maxVal, QWidget* parent)
    :
    InputGUI(label, parent),
    ui(new Ui::NumberGUI)
{
    ui->setupUi(this);
    _type = "number";

    _textLabel = qobject_cast<QLabel*>(ui->horizontalLayout->itemAt(0)->widget());
    _spinBox = qobject_cast<QDoubleSpinBox*>(ui->horizontalLayout->itemAt(1)->widget());
    ui->horizontalLayout->insertStretch(1);

    _spinBox->setMinimum(minVal);
    _spinBox->setMaximum(maxVal);

    connect(_spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &NumberGUI::valueChangedCallbacks);

    _textLabel->setText(QString(this->label().c_str()));
}

NumberGUI::~NumberGUI()
{
    delete ui;
}

void NumberGUI::addCallback(sol::function cb) {
    _callbacks.push_back(cb);
}

void NumberGUI::valueChangedCallbacks(double val) {
    for (const auto & callback : _callbacks) {
        callback(val);
    }
}

void NumberGUI::setLabel(const std::string& newLabel) {
    InputGUI::setLabel(newLabel);
    _textLabel->setText(QString(newLabel.c_str()));
}

double NumberGUI::value() const {
    return _spinBox->value();
}

void NumberGUI::setValue(double val) {
    _spinBox->setValue(val);
}

void NumberGUI::getLuaValue(sol::table & table) {
    table[_key] = value();
}

void NumberGUI::copyValue(QDataStream& stream) {
    stream << value();
}

void NumberGUI::pasteValue(QDataStream& stream) {
    double val;
    stream >> val;
    setValue(val);
}

void NumberGUI::hideLabel() {
    _textLabel->hide();
}
