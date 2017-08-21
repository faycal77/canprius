#ifndef CANBUSCONFIG_H
#define CANBUSCONFIG_H

#include <QDialog>

#include "canconnectionconfig.h"

class QGroupBox;
class QRadioButton;
class QPushButton;
class QComboBox;
class QDialogButtonBox;

namespace QPriusCan {

class CanBusConnectionDialog : public QDialog {
  Q_OBJECT
public:
  explicit CanBusConnectionDialog(QWidget *parent = Q_NULLPTR);

  CanConnectionConfig const &canConnectionConfig() const;

signals:

public slots:
  void okButtonClicked();

private:
  void createWidgets();
  void initWidgets();
  void createConnections();

  QRadioButton *m_radioButtonBase = Q_NULLPTR;
  QRadioButton *m_radioButtonExtented = Q_NULLPTR;
  QRadioButton *m_radioButtonBaseAndExtented = Q_NULLPTR;

  QDialogButtonBox *m_widgetButtons = Q_NULLPTR;

  QComboBox *m_canPluginComboxBox = Q_NULLPTR;
  QComboBox *m_canInterfaceComboxBox = Q_NULLPTR;
  QGroupBox *m_canFormatGroupBox = Q_NULLPTR;
  QGroupBox *m_canDeviceGroupBox = Q_NULLPTR;

  CanConnectionConfig m_canConnectionConfig;
};
}

#endif // CANBUSCONFIG_H
