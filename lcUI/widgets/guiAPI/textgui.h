#pragma once

#include "inputgui.h"
#include <QObject>
#include <QMetaObject>
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>

namespace Ui {
class TextGUI;
}

namespace lc {
namespace ui {
namespace api {
/**
* \brief Text GUI Widget
*/
class TextGUI : public InputGUI
{
    Q_OBJECT

public:
    /**
    * \brief TextGUI constructor
    * \param string gui label
    * \param parent qwidget parent
    */
    TextGUI(std::string label, QWidget* parent = nullptr);

    /**
    * \brief TextGUI destructor
    */
    ~TextGUI();

    /**
    * \brief Add callback for editingFinished
    * \param LuaRef lua callback
    */
    void addFinishCallback(sol::function cb);

    /**
    * \brief Add callback for textChanged
    * \param LuaRef lua callback
    */
    void addOnChangeCallback(sol::function cb);

    /**
    * \brief Overridden Set Text GUI widget label
    * \param string gui label
    */
    void setLabel(const std::string& newLabel) override;

    /**
    * \brief Return lineedit text
    * \return string text
    */
    std::string value() const;

    /**
    * \brief Set lineedit text
    * \param string text
    */
    void setValue(const std::string& newText);

    /**
    * \brief Return lua value
    * \param LuaRef value
    */
    void getLuaValue(sol::table & table) override;

    /**
    * \brief Hide widget label
    */
    void hideLabel() override;

public slots:
    /**
    * \brief Run callbacks for editing finished
    */
    void editingFinishedCallbacks();

    /**
    * \brief Run callbacks for text changed
    */
    void textChangedCallbacks(const QString& changedText);

protected:
    /**
    * \brief Copy widget value to the clipboard
    */
    void copyValue(QDataStream& stream) override;

    /**
    * \brief Set widget value from clipboard
    */
    void pasteValue(QDataStream& stream) override;

private:
    Ui::TextGUI* ui;
    QLabel* _textLabel;
    QLineEdit* _lineEdit;
    std::vector<sol::function> _callbacks_finished;
    std::vector<sol::function> _callbacks_onchange;
};
}
}
}
