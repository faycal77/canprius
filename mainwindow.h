#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCanBusDevice>
#include <QMainWindow>

class QActionGroup;
class QCanBusError;
class QAction;
class QTabWidget;
class QTableWidget;
class QPlainTextEdit;
class QCanBusFrame;
class QPushButton;

namespace QPriusCan {

class CanConnectionConfig;
class CanBusManager;
class MessageLogger;

class MainWindow : public QMainWindow {
  Q_OBJECT
  enum CanAction { Connect, Disconnect, Exit };

public:
  MainWindow(QWidget *parent = Q_NULLPTR, MessageLogger *logger = Q_NULLPTR);
  ~MainWindow();

private:
  void initApplication();
  void createWidgets();
  void createActions();
  void createConnections();
  void createMenu();

public slots:
  // GUI slots
  void handleSettingActions(QAction *action);
  void handleConnectionAction();
  void handleDisconnectionAction();
  void clearDataFrame();
  void clearErrorsFrame();
  // Can Data/Error slots
  void handleDataFrame(const QCanBusFrame &dataFrame);
  void handleErrorFrame(const QCanBusFrame &errorFrame);
  void handleError(QCanBusDevice::CanBusError error);
  // Can bus connection slots
  void canBusInitialised();
  void canBusConnected(bool connected);

protected:
  void createDataTabWidget();

  void createErrorTabWidget();

  void createFramesTabWidget();

private:
  QList<QAction *> m_canBusActions;
  QActionGroup *m_settingActions;

  QPushButton *m_clearDataFrameButton;
  QPushButton *m_clearErrorsFrameButton;

  CanBusManager *m_canBusManager;
  QTabWidget *m_framesTabWidget;
  QTableWidget *m_dataFrames;
  QTableWidget *m_errorsFrames;
  QPlainTextEdit *m_logPlainTextEdit;
  MessageLogger *m_logger;
};
}

#endif // MAINWINDOW_H
