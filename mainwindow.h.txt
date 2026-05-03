#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "bst.h" // Include logic

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAdd();
    void onAddSub();
    void onModify();
    void onDeleteAll();
    void onImport();
    void onExport();
    void onSearch(const QString &text);
    void onSortChanged();
    void onItemChanged(QTreeWidgetItem* item, int col);

private:
    // GUI Elements
    QTreeWidget* treeWidget;
    QLineEdit* searchBar;
    QLineEdit* titleIn;
    QSpinBox* prioIn;
    QDateEdit* dateIn;
    QComboBox* sortCombo;
    QLabel* statsLabel;

    // The Logic Object
    BST tree;

    // View Helpers (Connects Data to GUI)
    void refreshUI();
    void fillVisualTree(Task* node);
    void fillSubtasks(Task* subNode, QTreeWidgetItem* pItem);
    QColor getPriorityColor(int priority);
};

#endif // MAINWINDOW_H
