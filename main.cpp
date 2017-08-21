#include "mainwindow.h"
#include "messagelogger.h"

#include <QApplication>
#include <QFile>

using namespace QPriusCan;

int main(int argc, char *argv[]) {
  QFile logFile;
  // logFile.open(stdout, QIODevice::WriteOnly);
  logFile.setFileName("./gpflotte.log");
  MessageLogger logger;
  logger.installLoggerDevice(logFile);

  QApplication a(argc, argv);

  MainWindow w(Q_NULLPTR, &logger);
  w.show();

  return a.exec();
}
