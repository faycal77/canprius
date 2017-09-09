#ifndef CANBUSMANAGER_H
#define CANBUSMANAGER_H
#include <QCanBusDevice>
#include <QObject>

class QString;
class QCanBusFrame;

namespace QPriusCan {
class CanBusManager : public QObject {
  Q_OBJECT
public:
  CanBusManager(QObject *parent = Q_NULLPTR);
  bool initCanDevice(const QString &plugin, const QString &interfaceName,
                     QCanBusDevice::Filter::FormatFilter filterFormat =
                         QCanBusDevice::Filter::FormatFilter::MatchBaseFormat);
  bool connect();
  void disconnect();

public slots:
  void handleFramesReceived();
  void handleFramesWritten();
signals:
  void canBusInitialised();
  void canBusConnected(bool connect);
  void errorFrameReceived(QCanBusFrame const &data);
  void dataFrameReceived(QCanBusFrame const &data);
  void errorOccurred(QCanBusDevice::CanBusError error);

private:
  QCanBusDevice *m_canBusDevice = Q_NULLPTR;
  QString m_plugin;
  QString m_interfaceName;
};
}

#endif // CANBUSMANAGER_H
