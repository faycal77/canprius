#include "canbusmanager.h"
#include <QCanBus>
#include <QCanBusFrame>
#include <QDebug>

using namespace QPriusCan;

CanBusManager::CanBusManager(QObject *parent) : QObject(parent) {}

bool CanBusManager::initCanDevice(const QString &plugin,
                                  const QString &interfaceName) {
  QString error;
  m_canBusDevice =
      QCanBus::instance()->createDevice(plugin, interfaceName, &error);
  if (m_canBusDevice == Q_NULLPTR) {
    qWarning() << error;
    return false;
  } else {
    // set connection
    m_plugin = plugin;
    m_interfaceName = interfaceName;
    QObject::connect(m_canBusDevice, &QCanBusDevice::framesReceived, this,
                     &CanBusManager::handleFramesReceived);
    QObject::connect(m_canBusDevice, &QCanBusDevice::framesWritten, this,
                     &CanBusManager::handleFramesWritten);
    emit canBusInitialised();
  }
  return true;
}

bool CanBusManager::connect() {
  m_canBusDevice->setConfigurationParameter(QCanBusDevice::CanFdKey, true);
  if (m_plugin != QString("socketcan")) {
    m_canBusDevice->setConfigurationParameter(QCanBusDevice::BitRateKey,
                                              500000);
  }

  bool ret = m_canBusDevice->connectDevice();
  if (!ret) {
    qWarning()
        << tr("Can connection error : %1").arg(m_canBusDevice->errorString());
    delete m_canBusDevice;
    m_canBusDevice = nullptr;
    return false;
  } else {
    emit canBusConnected(true);
    qInfo() << tr("Connection etablished");
    qInfo() << m_canBusDevice->configurationParameter(QCanBusDevice::BitRateKey)
                   .toUInt();
  }
  return true;
}

void CanBusManager::disconnect() {
  if (m_canBusDevice != Q_NULLPTR &&
      m_canBusDevice->state() == QCanBusDevice::ConnectedState) {
    m_canBusDevice->disconnectDevice();
    delete m_canBusDevice;
    m_canBusDevice = nullptr;
    emit canBusConnected(false);
  }
}

void CanBusManager::handleFramesReceived() {
  qint64 nbFrames = m_canBusDevice->framesAvailable();
  for (qint64 iFrame = 0; iFrame < nbFrames; ++iFrame) {
    QCanBusFrame frame = m_canBusDevice->readFrame();
    if (frame.frameType() == QCanBusFrame::DataFrame) {
      emit dataFrameReceived(frame);
    }
    if (frame.frameType() == QCanBusFrame::ErrorFrame) {
      emit errorFrameReceived(frame);
    }
  }
}

void CanBusManager::handleFramesWritten() {
  qint64 nbFrames = m_canBusDevice->framesToWrite();
  qDebug() << " Frames writted : " << nbFrames;
}
