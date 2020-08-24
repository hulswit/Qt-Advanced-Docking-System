#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QWidgetAction>
#include <QLabel>
#include <QCalendarWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

#include "DockAreaWidget.h"
#include "DockAreaTitleBar.h"
#include "DockAreaTabBar.h"
#include "FloatingDockContainer.h"
#include "DockComponentsFactory.h"

using namespace ads;

const QString MainWindow::kTableTopLayout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<QtAdvancedDockingSystem Version=\"1\" UserVersion=\"0\" Containers=\"1\">"
            "<Container Floating=\"0\">"
                "<Splitter Orientation=\"-\" Count=\"3\">"
                    "<Area Tabs=\"1\" Current=\"Timeline\" AllowedAreas=\"f\" Flags=\"1\">"
                        "<Widget Name=\"Timeline\" Closed=\"0\" />"
                    "</Area>"
                    "<Splitter Orientation=\"|\" Count=\"3\">"
                        "<Splitter Orientation=\"-\" Count=\"2\">"
                            "<Area Tabs=\"1\" Current=\"File system\">"
                                "<Widget Name=\"File system\" Closed=\"0\"/>"
                            "</Area>"
                            "<Area Tabs=\"1\" Current=\"Table\">"
                                "<Widget Name=\"Table\" Closed=\"0\"/>"
                            "</Area>"
                            "<Sizes>344 272 </Sizes>"
                        "</Splitter>"
                        "<Area Tabs=\"1\" Current=\"\" AllowedAreas=\"f\" Flags=\"1\">"
                            "<Widget Name=\"CentralWidget\" Closed=\"0\"/>"
                        "</Area>"
                        "<Area Tabs=\"1\" Current=\"Properties\">"
                            "<Widget Name=\"Properties\" Closed=\"0\"/>"
                        "</Area>"
                        "<Sizes>258 758 258 </Sizes>"
                    "</Splitter>"
                    "<Area Tabs=\"1\" Current=\"Status\" AllowedAreas=\"f\" Flags=\"1\">"
                        "<Widget Name=\"Status\" Closed=\"0\"/>"
                    "</Area>"
                    "<Sizes>52 621 52 </Sizes>"
                "</Splitter>"
            "</Container>"
        "</QtAdvancedDockingSystem>";

const QString MainWindow::kTableBottomLayout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<QtAdvancedDockingSystem Version=\"1\" UserVersion=\"0\" Containers=\"1\">"
            "<Container Floating=\"0\">"
                "<Splitter Orientation=\"-\" Count=\"3\">"
                    "<Splitter Orientation=\"|\" Count=\"3\">"
                        "<Area Tabs=\"2\" Current=\"Table\">"
                            "<Widget Name=\"Table\" Closed=\"0\"/>"
                            "<Widget Name=\"File system\" Closed=\"0\"/>"
                        "</Area>"
                        "<Area Tabs=\"1\" Current=\"\" AllowedAreas=\"f\" Flags=\"1\">"
                            "<Widget Name=\"CentralWidget\" Closed=\"0\"/>"
                        "</Area>"
                        "<Area Tabs=\"1\" Current=\"Properties\">"
                            "<Widget Name=\"Properties\" Closed=\"0\"/>"
                        "</Area>"
                        "<Sizes>258 758 258 </Sizes>"
                    "</Splitter>"
                    "<Area Tabs=\"1\" Current=\"Timeline\" AllowedAreas=\"f\" Flags=\"1\">"
                        "<Widget Name=\"Timeline\" Closed=\"0\"/>"
                    "</Area>"
                    "<Area Tabs=\"1\" Current=\"Status\" AllowedAreas=\"f\" Flags=\"1\">"
                        "<Widget Name=\"Status\" Closed=\"0\"/>"
                    "</Area>"
                    "<Sizes>621 52 52 </Sizes>"
                "</Splitter>"
            "</Container>"
        "</QtAdvancedDockingSystem>";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, true);
    CDockManager::setConfigFlag(CDockManager::XmlCompressionEnabled, false);
    DockManager = new CDockManager(this);
    QCalendarWidget* calendar = new QCalendarWidget();
    CDockWidget* CentralDockWidget = new CDockWidget("CentralWidget");
    CentralDockWidget->setWidget(calendar);
    auto* CentralDockArea = DockManager->setCentralWidget(CentralDockWidget);
    CentralDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);

    QTreeView* fileTree = new QTreeView();
    fileTree->setFrameShape(QFrame::NoFrame);
    QFileSystemModel* fileModel = new QFileSystemModel(fileTree);
    fileModel->setRootPath(QDir::currentPath());
    fileTree->setModel(fileModel);
    CDockWidget* DataDockWidget = new CDockWidget("File system");
    DataDockWidget->setWidget(fileTree);
    DataDockWidget->resize(150, 250);
    DataDockWidget->setMinimumSize(100, 250);
    auto* fileArea = DockManager->addDockWidget(DockWidgetArea::LeftDockWidgetArea, DataDockWidget, CentralDockArea);

    QTableWidget* table = new QTableWidget();
    table->setColumnCount(3);
    table->setRowCount(10);
    CDockWidget* TableDockWidget = new CDockWidget("Table");
    TableDockWidget->setWidget(table);
    TableDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    TableDockWidget->resize(250, 150);
    TableDockWidget->setMinimumSize(200,150);
    DockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, TableDockWidget, fileArea);

    QTableWidget* propertiesTable = new QTableWidget();
    table->setColumnCount(3);
    table->setRowCount(10);
    CDockWidget* PropertiesDockWidget = new CDockWidget("Properties");
    PropertiesDockWidget->setWidget(propertiesTable);
    PropertiesDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    PropertiesDockWidget->resize(250, 150);
    PropertiesDockWidget->setMinimumSize(200,150);
    DockManager->addDockWidget(DockWidgetArea::RightDockWidgetArea, PropertiesDockWidget, CentralDockArea);

    QWidget* timeLineWidget = new QWidget();
    QHBoxLayout* timelineLayout = new QHBoxLayout(timeLineWidget);
    QRadioButton* radioDockTop = new QRadioButton("Top", timeLineWidget);
    QRadioButton* radioDockBottom = new QRadioButton("Bottom", timeLineWidget);
    radioDockTop->setChecked(true);
    timelineLayout->addWidget(new QLabel("Test Widget."));
    timelineLayout->addStretch(1);
    timelineLayout->addWidget(new QLabel("Apply predefined perspective: ", this));
    timelineLayout->addWidget(radioDockTop);
    timelineLayout->addWidget(radioDockBottom);
    TimelineDockWidget = new CDockWidget("Timeline");
    TimelineDockWidget->setWidget(timeLineWidget);
    TimelineDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    TimelineDockWidget->setFeature(CDockWidget::DockWidgetMovable, false);
    TimelineDockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);
    TimelineDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    TimelineDockWidget->setMinimumSize(QSize(50, 50));
    auto *TimelineDockArea = DockManager->addDockWidget(DockWidgetArea::TopDockWidgetArea, TimelineDockWidget);
    TimelineDockArea->setDockAreaFlag(CDockAreaWidget::eDockAreaFlag::HideSingleWidgetTitleBar, true);
    TimelineDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);
    connect(radioDockTop, &QRadioButton::toggled, [&](bool checked){
        bool ok = true;
        if(!checked)
        {
            ok = DockManager->restoreState(kTableBottomLayout.toUtf8());
        }
        else
        {
            ok = DockManager->restoreState(kTableTopLayout.toUtf8());
        }
        if(!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Failed to apply perspective!");
            msgBox.exec();
        }
    });

    QWidget* statusWidget = new QWidget();
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setSpacing(10);
    statusLayout->addWidget(new QLabel("Status Bar"));
    QPushButton* OpenPerspectiveButton = new QPushButton("Open Perspective", statusWidget);
    connect(OpenPerspectiveButton, &QPushButton::clicked, [&](){
        QString PerspectiveName = QFileDialog::getOpenFileName(this, "Open Perspective", "", "Perspective files (*.xml)");
        if (PerspectiveName.isEmpty())
        {
            return;
        }

        QFile stateFile(PerspectiveName);
        stateFile.open(QIODevice::ReadOnly);
        QByteArray state = stateFile.readAll();
        stateFile.close();
        if(!DockManager->restoreState(state))
        {
            QMessageBox msgBox;
            msgBox.setText("Failed to apply perspective " + stateFile.fileName());
            msgBox.exec();
        }
    });
    QPushButton* SavePerspectiveButton = new QPushButton("Create Perspective", statusWidget);
    connect(SavePerspectiveButton, &QPushButton::clicked, [&](){
        QString PerspectiveName = QInputDialog::getText(this, "Save Perspective", "Enter unique name:");
        if (PerspectiveName.isEmpty())
        {
            return;
        }

        QByteArray state = DockManager->saveState();
        QFile stateFile(PerspectiveName + ".xml");
        stateFile.open(QIODevice::WriteOnly);
        stateFile.write(state);
        stateFile.close();
    });
    statusLayout->addWidget(OpenPerspectiveButton);
    statusLayout->addWidget(SavePerspectiveButton);
    statusLayout->addStretch(1);
    CDockWidget* StatusDockWidget = new CDockWidget("Status");
    StatusDockWidget->setWidget(statusWidget);
    StatusDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    StatusDockWidget->setFeature(CDockWidget::DockWidgetMovable, false);
    StatusDockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);
    StatusDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    StatusDockWidget->setMinimumSize(QSize(50, 50));
    StatusDockArea = DockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, StatusDockWidget);
    StatusDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);
    StatusDockArea->setDockAreaFlag(ads::CDockAreaWidget::eDockAreaFlag::HideSingleWidgetTitleBar, true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

