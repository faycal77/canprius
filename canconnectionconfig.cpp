#include "canconnectionconfig.h"

using namespace QPriusCan;

CanConnectionConfig::CanConnectionConfig() {}

QString CanConnectionConfig::pluginName() const { return m_pluginName; }

void CanConnectionConfig::setPluginName(const QString &pluginName) {
  m_pluginName = pluginName;
}

QString CanConnectionConfig::deviceInterface() const {
  return m_deviceInterface;
}

void CanConnectionConfig::setDeviceInterface(const QString &deviceInterface) {
  m_deviceInterface = deviceInterface;
}

QCanBusDevice::Filter::FormatFilter CanConnectionConfig::FrameFormat() const {
  return m_FrameFormat;
}

void CanConnectionConfig::setFrameFormat(
    const QCanBusDevice::Filter::FormatFilter &frameFormat) {
  m_FrameFormat = frameFormat;
}
