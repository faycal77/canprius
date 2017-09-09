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

  QCanBusDevice::Filter::FormatFilter FrameFormat() const;
  void setFrameFormat(const QCanBusDevice::Filter::FormatFilter &frameFormat);

private:
  QString m_pluginName;
  QString m_deviceInterface;
  QCanBusDevice::Filter::FormatFilter m_FrameFormat;
};
}

#endif // CANCONNECTIONCONFIG_H
