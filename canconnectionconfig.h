#ifndef CANCONNECTIONCONFIG_H
#define CANCONNECTIONCONFIG_H

#include <QCanBusDevice>
#include <QString>

namespace QPriusCan {
class CanConnectionConfig {
public:
  CanConnectionConfig();

  QString pluginName() const;
  void setPluginName(const QString &pluginName);

  QString deviceInterface() const;
  void setDeviceInterface(const QString &deviceInterface);

  QCanBusDevice::Filter FrameFormat() const;
  void setFrameFormat(const QCanBusDevice::Filter &FrameFormat);

private:
  QString m_pluginName;
  QString m_deviceInterface;
  QCanBusDevice::Filter m_FrameFormat;
};
}

#endif // CANCONNECTIONCONFIG_H
