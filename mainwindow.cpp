#include "mainwindow.h"
#include "canbusconnectiondialog.h"
#include "canbusmanager.h"
#include "canconnectionconfig.h"
#include "messagelogger.h"
#include "priusodbcodes.h"

#include <QAction>
#include <QActionGroup>
#include <QCanBus>
#include <QDebug>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolBar>

using namespace QPriusCan;

MainWindow::MainWindow(QWidget *parent, MessageLogger *logger)
    : QMainWindow(parent), m_logger(logger) {
  initApplication();
  createWidgets();
  createActions();
  createConnections();
  createMenu();
}

MainWindow::~MainWindow() {}

void MainWindow::initApplication() {
  this->resize(QSize(QApplication::desktop()->availableGeometry().width(),
                     QApplication::desktop()->availableGeometry().height()));
}

void MainWindow::createWidgets() {
  m_framesTabWidget = new QTabWidget;
  m_dataFrames = new QTableWidget;
  m_errorsFrames = new QTableWidget;

  m_framesTabWidget->addTab(m_dataFrames, QString("Data frames"));
  m_framesTabWidget->addTab(m_errorsFrames, QString("Error frames"));
  m_logPlainTextEdit = new QPlainTextEdit;
  m_logPlainTextEdit->setReadOnly(true);
  m_logPlainTextEdit->setTextInteractionFlags(
      m_logPlainTextEdit->textInteractionFlags() & ~Qt::TextEditable);

  QDockWidget *dockWidget = new QDockWidget;
  dockWidget->setWindowTitle(tr("Logs :"));
  dockWidget->setFeatures(QDockWidget::DockWidgetFloatable |
                          QDockWidget::DockWidgetMovable);
  dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
  dockWidget->setWidget(m_logPlainTextEdit);
  addDockWidget(Qt::BottomDockWidgetArea, dockWidget);

  setCentralWidget(m_framesTabWidget);
}

void MainWindow::createActions() {
  m_canBusActions << new QAction(tr("Connect")) << new QAction(tr("Disconnect"))
                  << new QAction(tr("Exit"));

  m_settingActions = new QActionGroup(this);
  m_settingActions->addAction(new QAction(tr("Connection")));
  m_canBusActions[MainWindow::Connect]->setEnabled(false);
  m_canBusActions[MainWindow::Disconnect]->setEnabled(false);
}

void MainWindow::createConnections() {
  connect(m_settingActions, &QActionGroup::triggered, this,
          &MainWindow::handleSettingActions);
  connect(m_canBusActions[MainWindow::Connect], &QAction::triggered, this,
          &MainWindow::handleConnectionAction);
  connect(m_canBusActions[MainWindow::Disconnect], &QAction::triggered, this,
          &MainWindow::handleDisconnectionAction);
  connect(m_canBusActions[MainWindow::Exit], &QAction::triggered, this,
          &MainWindow::close);
  if (m_logger) {
    connect(m_logger, &MessageLogger::logMessage, m_logPlainTextEdit,
            &QPlainTextEdit::appendPlainText);
  }
}

void MainWindow::createMenu() {
  QMenu *canMenu = menuBar()->addMenu(tr("Can actions"));
  canMenu->addActions(m_canBusActions);

  QMenu *configuration = menuBar()->addMenu(tr("Settings"));
  configuration->addActions(m_settingActions->actions());
}

void MainWindow::handleSettingActions(QAction *action) {
  if (action->iconText() == tr("Connection")) {
    CanBusConnectionDialog *d = new CanBusConnectionDialog;
    if (d->exec() == QMessageBox::Accepted) {
      QString error;
      m_canBusManager = new CanBusManager(this);
      connect(m_canBusManager, &CanBusManager::canBusInitialised, this,
              &MainWindow::canBusInitialised);
      connect(m_canBusManager, &CanBusManager::canBusConnected, this,
              &MainWindow::canBusConnected);
      connect(m_canBusManager, &CanBusManager::errorOccurred, this,
              &MainWindow::handleError);

      m_canBusManager->initCanDevice(
          d->canConnectionConfig().pluginName(),
          d->canConnectionConfig().deviceInterface());
    }
  }
}

void MainWindow::handleConnectionAction() {
  if (m_canBusManager != Q_NULLPTR) {
    if (m_canBusManager->connect()) {
      // make connection
      connect(m_canBusManager, &CanBusManager::dataFrameReceived, this,
              &MainWindow::handleDataFrame);
      connect(m_canBusManager, &CanBusManager::errorFrameReceived, this,
              &MainWindow::handleErrorFrame);
    }
  }
}

void MainWindow::handleDisconnectionAction() { m_canBusManager->disconnect(); }

void MainWindow::handleErrorFrame(const QCanBusFrame &errorFrame) {
  qWarning() << errorFrame.frameId() << errorFrame.frameType()
             << errorFrame.isValid() << errorFrame.timeStamp().microSeconds()
             << errorFrame.payload();
}

void MainWindow::handleDataFrame(const QCanBusFrame &dataFrame) {
  switch (dataFrame.frameId()) {
  case PriusDataODBCode::DoorsId:
    qInfo() << " Doors : " << dataFrame.payload();
    break;
  case PriusDataODBCode::BatteryVolyageId:
    qInfo() << " Battery : " << dataFrame.payload();
    break;
  case PriusDataODBCode::BatterySocId:
    qInfo() << " Battery Soc : " << dataFrame.payload();
    break;
  case PriusDataODBCode::BrakesId:
    qInfo() << " Brake : " << dataFrame.payload();
    break;
  case PriusDataODBCode::EngineTemperatureId:
    qInfo() << " Temeprature : " << dataFrame.payload();
    break;
  default:
    qInfo() << dataFrame.frameId() << dataFrame.frameType()
            << dataFrame.isValid() << dataFrame.timeStamp().microSeconds()
            << dataFrame.payload();
  }
}

void MainWindow::handleError(QCanBusDevice::CanBusError error) {
  Q_UNUSED(error);
}

void MainWindow::canBusInitialised() {
  m_canBusActions[MainWindow::Connect]->setEnabled(true);
  m_settingActions->setDisabled(true);
}

void MainWindow::canBusConnected(bool connected) {
  if (connected) {
    m_canBusActions[MainWindow::Connect]->setEnabled(false);
    m_canBusActions[MainWindow::Disconnect]->setEnabled(true);
  } else {
    m_settingActions->setEnabled(true);
    m_canBusActions[MainWindow::Connect]->setEnabled(false);
    m_canBusActions[MainWindow::Disconnect]->setEnabled(false);
  }
}
