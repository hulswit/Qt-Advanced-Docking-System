#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "DockWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    static const QString kTableTopLayout;
    static const QString kTableBottomLayout;

    Ui::MainWindow *ui;

    ads::CDockManager* DockManager;
    ads::CDockAreaWidget* StatusDockArea;
    ads::CDockWidget* TimelineDockWidget;
};
#endif // MAINWINDOW_H
