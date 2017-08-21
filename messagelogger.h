#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QApplication>
#include <QTextStream>

class QIODevice;

namespace QPriusCan {
class MessageLogger : public QObject {
  Q_OBJECT
public:
  MessageLogger();
  bool installLoggerDevice(QIODevice &device);
  bool desinstallLoggerDevice();
  virtual ~MessageLogger();
  void emitMessage(QString const &message);
signals:
  void logMessage(QString const &message);

private:
  static void messageHandler(QtMsgType type, const QMessageLogContext &context,
                             const QString &msg);

  static QTextStream m_textStream;
  bool m_opened = false;
  static MessageLogger *logger;
};
}

#endif // MESSAGELOGGER_H
