#include "messagelogger.h"

#include <QTextStream>
#include <QTime>

using namespace QPriusCan;

QTextStream MessageLogger::m_textStream;
MessageLogger *MessageLogger::logger = Q_NULLPTR;

void MessageLogger::messageHandler(QtMsgType type,
                                   const QMessageLogContext &context,
                                   const QString &msg) {

  QByteArray message = msg.toLocal8Bit();
  switch (type) {
  case QtDebugMsg: {
    QString messageString =
        QString("[ Debug ] : %5 -- %1 (%2:%3, %4)")
            .arg(message.constData(), QString(context.file),
                 QString::number(context.line), QString(context.function),
                 QTime::currentTime().toString());
    m_textStream << messageString << endl;
    m_textStream.flush();
    if (logger)
      logger->emitMessage(messageString);
  } break;
  case QtInfoMsg: {
    QString messageString =
        QString("[ Info ] : %2 -- %1 ")
            .arg(message.constData(), QTime::currentTime().toString());
    m_textStream << messageString << endl;
    m_textStream.flush();
    if (logger)
      logger->emitMessage(messageString);
  } break;
  case QtWarningMsg: {
    QString messageString =
        QString("[ Warning ] : %5 -- %1 (%2:%3, %4)")
            .arg(message.constData(), QString(context.file),
                 QString::number(context.line), QString(context.function),
                 QTime::currentTime().toString());
    m_textStream << messageString << endl;
    m_textStream.flush();
    if (logger)
      logger->emitMessage(messageString);
  } break;
  case QtCriticalMsg: {
    QString messageString =
        QString("[ Critical ] : %5 -- %1 (%2:%3, %4)")
            .arg(message.constData(), QString(context.file),
                 QString::number(context.line), QString(context.function),
                 QTime::currentTime().toString());
    m_textStream << messageString << endl;
    m_textStream.flush();
    if (logger)
      logger->emitMessage(messageString);
  } break;
  case QtFatalMsg: {
    QString messageString =
        QString("[ Fatal ] : %5 -- %1 (%2:%3, %4)")
            .arg(message.constData(), QString(context.file),
                 QString::number(context.line), QString(context.function),
                 QTime::currentTime().toString());
    m_textStream << messageString << endl;
    m_textStream.flush();
    if (logger)
      logger->emitMessage(messageString);
    abort();
  }
  }
}

MessageLogger::MessageLogger() { logger = this; }

bool MessageLogger::installLoggerDevice(QIODevice &device) {
  m_opened = false;
  if (!device.isOpen()) {
    m_opened = device.open(QIODevice::WriteOnly);
  }
  if (m_opened || device.isOpen()) {
    m_textStream.setDevice(&device);
    qInstallMessageHandler(MessageLogger::messageHandler);
  }
  return true;
}

bool MessageLogger::desinstallLoggerDevice() {
  if (m_opened && m_textStream.device()) {
    m_textStream.device()->close();
  }
  return true;
}

MessageLogger::~MessageLogger() { desinstallLoggerDevice(); }

void MessageLogger::emitMessage(const QString &message) {
  emit logMessage(message);
}
