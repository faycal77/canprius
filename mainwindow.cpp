#include "mainwindow.h"
#include "canbusconnectiondialog.h"
#include "canbusmanager.h"
#include "canconnectionconfig.h"
#include "messagelogger.h"
#include "priusodbcodes.h"

#include <QAction>
#include <QActionGroup>
#include <QCanBus>
#include <QDateTime>
#include <QDebug>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QHeaderView>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSizePolicy>
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

void MainWindow::createDataTabWidget() {
  m_dataFrames = new QTableWidget;
  m_dataFrames->setShowGrid(false);
  m_dataFrames->setColumnCount(4);
  QStringList labels;
  labels << QString("Id") << QString("Date") << QString("Time stamp")
         << QString("Received data");
  m_dataFrames->setHorizontalHeaderLabels(labels);
  m_dataFrames->horizontalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
}

void MainWindow::createErrorTabWidget() { m_errorsFrames = new QTableWidget; }

void MainWindow::createFramesTabWidget() {
  m_clearDataFrameButton = new QPushButton("clear");
  m_clearErrorsFrameButton = new QPushButton("clear");
  m_clearDataFrameButton->setIcon(QIcon::fromTheme("edit-clear"));
  m_clearErrorsFrameButton->setIcon(QIcon::fromTheme("edit-clear"));
  m_clearDataFrameButton->setFlat(true);
  m_clearErrorsFrameButton->setFlat(true);
  m_framesTabWidget = new QTabWidget;
  m_framesTabWidget->addTab(m_dataFrames, QIcon::fromTheme("network-wired"),
                            QString("Data frames"));
  m_framesTabWidget->tabBar()->setTabButton(
      0, QTabBar::ButtonPosition::RightSide, m_clearDataFrameButton);
  m_framesTabWidget->addTab(m_errorsFrames, QIcon::fromTheme("dialog-error"),
                            QString("Error frames"));
  m_framesTabWidget->tabBar()->setTabButton(
      1, QTabBar::ButtonPosition::RightSide, m_clearErrorsFrameButton);
}

void MainWindow::createWidgets() {
  createDataTabWidget();
  createErrorTabWidget();
  createFramesTabWidget();
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
  m_canBusActions
      << new QAction(QIcon::fromTheme("network-wireless"), tr("Connect"))
      << new QAction(QIcon::fromTheme("window-close"), tr("Disconnect"))
      << new QAction(QIcon::fromTheme("application-exit"), tr("Exit"));

  m_settingActions = new QActionGroup(this);
  m_settingActions->addAction(
      new QAction(QIcon::fromTheme("preferences-system"), tr("Connection")));
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
  connect(m_clearDataFrameButton, &QPushButton::clicked, this,
          &MainWindow::clearDataFrame);
  connect(m_clearErrorsFrameButton, &QPushButton::clicked, this,
          &MainWindow::clearErrorsFrame);

  connect(m_clearErrorsFrameButton, &QPushButton::clicked, m_errorsFrames,
          &QTableWidget::clearContents);

  if (m_logger) {
    connect(m_logger, &MessageLogger::logMessage, m_logPlainTextEdit,
            &QPlainTextEdit::appendPlainText);
  }
}

void MainWindow::createMenu() {
  QMenu *canMenu = menuBar()->addMenu(tr("CAN actions"));
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

      m_canBusManager->initCanDevice(d->canConnectionConfig().pluginName(),
                                     d->canConnectionConfig().deviceInterface(),
                                     d->canConnectionConfig().FrameFormat());
    }
    delete d;
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

void MainWindow::handleDisconnectionAction() {
  m_canBusManager->disconnect();
  m_logPlainTextEdit->clear();
}

void MainWindow::clearDataFrame() { m_dataFrames->setRowCount(0); }

void MainWindow::clearErrorsFrame() { m_errorsFrames->setRowCount(0); }

void MainWindow::handleErrorFrame(const QCanBusFrame &errorFrame) {
  qWarning() << errorFrame.frameId() << errorFrame.frameType()
             << errorFrame.isValid() << errorFrame.timeStamp().microSeconds()
             << errorFrame.payload().toHex();
}

void MainWindow::handleDataFrame(const QCanBusFrame &dataFrame) {
  if (!dataFrame.isValid())
    return;

  QTableWidgetItem *messageItem = new QTableWidgetItem;
  QTableWidgetItem *idItem = new QTableWidgetItem;
  QTableWidgetItem *dateItem = new QTableWidgetItem;
  QTableWidgetItem *timeItem = new QTableWidgetItem;
  QString itemData;

  dateItem->setData(Qt::DisplayRole, QDateTime::currentDateTime());
  bool unreconized = false;
  idItem->setIcon(QIcon::fromTheme("help-about"));

  switch (dataFrame.frameId()) {
  case QPriusCanCode::DoorsId:
    itemData += " Doors : ";
    itemData += dataFrame.payload().toHex('.').toUpper();
    messageItem->setData(Qt::DisplayRole, itemData);
    idItem->setData(Qt::DisplayRole, QPriusCanCode::DoorsId);
    timeItem->setData(Qt::DisplayRole, dataFrame.timeStamp().microSeconds());
    break;
  case QPriusCanCode::BatteryVolyageId:
    itemData += " Battery : ";
    break;
  case QPriusCanCode::BatterySocId:
    itemData += " Battery Soc : ";
    break;
  case QPriusCanCode::BrakesId:
    itemData += " Brake : ";
    break;
  case QPriusCanCode::EngineTemperatureId:
    itemData += " Temeprature : ";
    break;
  default:
    unreconized = true;
  }
  // add the item to the data table
  if (!unreconized) {
    m_dataFrames->insertRow(m_dataFrames->rowCount());
    m_dataFrames->setItem(m_dataFrames->rowCount() - 1, 0, idItem);
    m_dataFrames->setItem(m_dataFrames->rowCount() - 1, 1, dateItem);
    m_dataFrames->setItem(m_dataFrames->rowCount() - 1, 2, timeItem);
    m_dataFrames->setItem(m_dataFrames->rowCount() - 1, 3, messageItem);

    m_dataFrames->scrollToBottom();
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
