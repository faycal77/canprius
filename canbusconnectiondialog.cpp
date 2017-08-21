#include "canbusconnectiondialog.h"
#include "canconnectionconfig.h"

#include <QCanBus>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QNetworkInterface>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

using namespace QPriusCan;

CanBusConnectionDialog::CanBusConnectionDialog(QWidget *parent)
    : QDialog(parent) {
  createWidgets();
  initWidgets();
  createConnections();
}

void CanBusConnectionDialog::okButtonClicked() {
  m_canConnectionConfig.setDeviceInterface(
      m_canInterfaceComboxBox->currentText());
  m_canConnectionConfig.setPluginName(m_canPluginComboxBox->currentText());
  accept();
}

void CanBusConnectionDialog::createWidgets() {
  m_canFormatGroupBox = new QGroupBox(tr("Can bus frame format"));
  m_radioButtonBase = new QRadioButton(tr("Base"));
  m_radioButtonExtented = new QRadioButton(tr("Extented"));
  m_radioButtonBaseAndExtented = new QRadioButton(tr("Base and Extended"));

  m_canDeviceGroupBox = new QGroupBox(tr("Can bus device"));
  m_canPluginComboxBox = new QComboBox;
  m_canInterfaceComboxBox = new QComboBox;
  QLabel *pluginLabel = new QLabel(tr("Plugin : "));
  QLabel *interfaceLabel = new QLabel(tr("Interface : "));

  m_widgetButtons =
      new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

  QVBoxLayout *canFormatGroupBoxLayout = new QVBoxLayout;
  QVBoxLayout *canDeviceComboBoxLayout = new QVBoxLayout;
  QGridLayout *hPluginBoxLayout = new QGridLayout;

  QHBoxLayout *groupBoxHBoxLayout = new QHBoxLayout;
  QVBoxLayout *widgetLayout = new QVBoxLayout;

  hPluginBoxLayout->addWidget(pluginLabel, 0, 0);
  hPluginBoxLayout->addWidget(m_canPluginComboxBox, 0, 1);
  hPluginBoxLayout->addWidget(interfaceLabel, 1, 0);
  hPluginBoxLayout->addWidget(m_canInterfaceComboxBox, 1, 1);

  canDeviceComboBoxLayout->addLayout(hPluginBoxLayout);
  canFormatGroupBoxLayout->addWidget(m_radioButtonBase);
  canFormatGroupBoxLayout->addWidget(m_radioButtonExtented);
  canFormatGroupBoxLayout->addWidget(m_radioButtonBaseAndExtented);
  canFormatGroupBoxLayout->addStretch(1);

  m_canFormatGroupBox->setLayout(canFormatGroupBoxLayout);
  m_canDeviceGroupBox->setLayout(canDeviceComboBoxLayout);

  groupBoxHBoxLayout->addWidget(m_canFormatGroupBox);
  groupBoxHBoxLayout->addWidget(m_canDeviceGroupBox);

  widgetLayout->addLayout(groupBoxHBoxLayout);
  widgetLayout->addWidget(m_widgetButtons);
  setLayout(widgetLayout);
}

void CanBusConnectionDialog::initWidgets() {
  m_radioButtonBase->setChecked(true);
  m_canPluginComboxBox->addItems(QCanBus::instance()->plugins());
  foreach (QNetworkInterface const &interface,
           QNetworkInterface::allInterfaces()) {
    if ((interface.flags() & QNetworkInterface::IsUp) &&
        !(interface.flags() & QNetworkInterface::IsLoopBack) &&
        !(interface.flags() & QNetworkInterface::CanMulticast))
      m_canInterfaceComboxBox->addItem(interface.name());
  }
  m_widgetButtons->button(QDialogButtonBox::Ok)
      ->setEnabled(m_canPluginComboxBox->count() &&
                   m_canInterfaceComboxBox->count());
}

void CanBusConnectionDialog::createConnections() {
  connect(m_widgetButtons, &QDialogButtonBox::rejected, this,
          &CanBusConnectionDialog::close);

  connect(m_widgetButtons->button(QDialogButtonBox::Ok), &QPushButton::clicked,
          this, &CanBusConnectionDialog::okButtonClicked);
}

CanConnectionConfig const &CanBusConnectionDialog::canConnectionConfig() const {
  return m_canConnectionConfig;
}
