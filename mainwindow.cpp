#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QDebug>

// =========================================================
//  GUI IMPLEMENTATION ONLY
//  (All Logic is now in bst.h)
// =========================================================

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // 1. Setup UI Components
    treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabels({"Task Name", "Priority", "Due Date", "Done"});
    treeWidget->setColumnWidth(1, 80);

    searchBar = new QLineEdit();
    searchBar->setPlaceholderText("Search (Type EXACT name)...");

    titleIn = new QLineEdit(); titleIn->setPlaceholderText("Task Title...");
    prioIn = new QSpinBox(); prioIn->setRange(1, 10);
    dateIn = new QDateEdit(QDate::currentDate());

    sortCombo = new QComboBox();
    sortCombo->addItem("Sort by Priority");
    sortCombo->addItem("Sort by Date");

    statsLabel = new QLabel("Main Tasks: 0 | Subtasks: 0");
    statsLabel->setStyleSheet("font-weight: bold; color: #444;");

    QPushButton* btnAdd = new QPushButton("Add Task");
    QPushButton* btnSub = new QPushButton("Add Subtask");
    QPushButton* btnModify = new QPushButton("Modify");
    QPushButton* btnDel = new QPushButton("Clear All");
    QPushButton* btnImp = new QPushButton("Import");
    QPushButton* btnExp = new QPushButton("Export");

    // 2. Layouts
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(searchBar);

    QHBoxLayout* r1 = new QHBoxLayout();
    r1->addWidget(titleIn); r1->addWidget(prioIn); r1->addWidget(dateIn); r1->addWidget(btnAdd);
    layout->addLayout(r1);

    layout->addWidget(sortCombo);
    layout->addWidget(treeWidget);
    layout->addWidget(statsLabel);

    QHBoxLayout* r2 = new QHBoxLayout();
    r2->addWidget(btnSub); r2->addWidget(btnModify); r2->addWidget(btnDel); r2->addWidget(btnImp); r2->addWidget(btnExp);
    layout->addLayout(r2);

    setCentralWidget(new QWidget); centralWidget()->setLayout(layout);

    // 3. Connect Signals to Slots
    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::onAdd);
    connect(btnModify, &QPushButton::clicked, this, &MainWindow::onModify);
    connect(btnSub, &QPushButton::clicked, this, &MainWindow::onAddSub);
    connect(btnDel, &QPushButton::clicked, this, &MainWindow::onDeleteAll);
    connect(btnImp, &QPushButton::clicked, this, &MainWindow::onImport);
    connect(btnExp, &QPushButton::clicked, this, &MainWindow::onExport);
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::onSearch);
    connect(sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onSortChanged);
    connect(treeWidget, &QTreeWidget::itemChanged, this, &MainWindow::onItemChanged);

    cout << "=== APP STARTED ===" << endl;
    cout << "Logic Loaded from BST.h" << endl;
}

MainWindow::~MainWindow() {
    // Optional: Print one last time on close
    tree.printConsole(sortCombo->currentIndex());
}

// =========================================================
//  BUTTON SLOTS (Interactions)
// =========================================================

void MainWindow::onAdd() {
    if(titleIn->text().isEmpty()) return;
    // Call Logic
    tree.insert(new Task(titleIn->text(), prioIn->value(), dateIn->date()), sortCombo->currentIndex());
    titleIn->clear();
    refreshUI();
}

void MainWindow::onAddSub() {
    auto sel = treeWidget->currentItem();
    if(!sel || titleIn->text().isEmpty()) return;

    // Logic: Find Parent -> Add Sub
    Task* p = tree.find(sel->text(0));
    if(p) tree.addSubtask(p, new Task(titleIn->text(), prioIn->value(), dateIn->date()), sortCombo->currentIndex());

    titleIn->clear();
    refreshUI();
}

void MainWindow::onModify() {
    auto sel = treeWidget->currentItem();
    if (!sel) return;
    Task* t = tree.find(sel->text(0));
    if (!t) return;

    // Dialog is GUI element, so it stays here
    QDialog diag(this);
    diag.setWindowTitle("Edit Task");
    QVBoxLayout* dL = new QVBoxLayout(&diag);
    QLineEdit* eT = new QLineEdit(t->title);
    QSpinBox* eP = new QSpinBox(); eP->setRange(1, 10); eP->setValue(t->priority);
    QDateEdit* eD = new QDateEdit(t->date);
    QPushButton* bS = new QPushButton("Save");
    dL->addWidget(new QLabel("Name:")); dL->addWidget(eT);
    dL->addWidget(new QLabel("Priority:")); dL->addWidget(eP);
    dL->addWidget(new QLabel("Date:")); dL->addWidget(eD);
    dL->addWidget(bS);
    connect(bS, &QPushButton::clicked, &diag, &QDialog::accept);

    if (diag.exec() == QDialog::Accepted) {
        t->title = eT->text(); t->priority = eP->value(); t->date = eD->date();
        // Since data changed, we might need to re-sort
        onSortChanged();
    }
}

void MainWindow::onDeleteAll() {
    tree.clear();
    refreshUI();
}

void MainWindow::onSortChanged() {
    tree.rebuild(sortCombo->currentIndex());
    refreshUI();
}

void MainWindow::onExport() {
    QString p = QFileDialog::getSaveFileName(this, "Export", "", "CSV (*.csv)");
    if(!p.isEmpty()) tree.exportToCSV(p);
}

void MainWindow::onImport() {
    QString p = QFileDialog::getOpenFileName(this, "Import", "", "CSV (*.csv)");
    if(!p.isEmpty()) {
        tree.importFromCSV(p, sortCombo->currentIndex());
        refreshUI();
    }
}

// === FIX IS HERE ===
void MainWindow::onSearch(const QString &text) {
    treeWidget->clearSelection();
    if (text.isEmpty()) return;
    Task* found = tree.find(text);
    if (found) {
        auto items = treeWidget->findItems(found->title, Qt::MatchExactly | Qt::MatchRecursive);
        // FIX: Removed qAsConst() -> Just iterate 'items' directly
        for (auto* item : items) {
            item->setSelected(true);
            treeWidget->scrollToItem(item);
        }
    }
}

void MainWindow::onItemChanged(QTreeWidgetItem* item, int col) {
    if (treeWidget->signalsBlocked()) return;
    // Handle Checkbox logic (Deletion)
    if (col == 3 && item->checkState(3) == Qt::Checked) {
        if (item->parent()) {
            tree.removeSubtask(tree.find(item->parent()->text(0)), item->text(0), sortCombo->currentIndex());
        } else {
            tree.removeMain(item->text(0), sortCombo->currentIndex());
        }
        refreshUI();
    }
}

// =========================================================
//  VIEW UPDATERS (Connecting Data to GUI)
// =========================================================

void MainWindow::refreshUI() {
    treeWidget->blockSignals(true);
    treeWidget->clear();

    // Map Logic Data to GUI Widgets
    fillVisualTree(tree.root);

    treeWidget->expandAll();

    // Get Stats from Logic
    statsLabel->setText(QString("Tasks: %1 | Subtasks: %2").arg(tree.countMain()).arg(tree.countSub()));
    treeWidget->blockSignals(false);

    // Trigger Console Print (Logic)
    tree.printConsole(sortCombo->currentIndex());
}

// Recursively fill QTreeWidget (This is View Logic, so it stays in MainWindow)
void MainWindow::fillVisualTree(Task* node) {
    if(!node) return;
    fillVisualTree(node->left);

    QTreeWidgetItem* it = new QTreeWidgetItem(treeWidget);
    it->setText(0, node->title);
    it->setText(1, QString::number(node->priority));
    it->setText(2, node->date.toString("yyyy-MM-dd"));
    it->setBackground(1, getPriorityColor(node->priority));
    it->setForeground(1, Qt::black);
    it->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    it->setCheckState(3, Qt::Unchecked);

    fillSubtasks(node->subtasksRoot, it);
    fillVisualTree(node->right);
}

void MainWindow::fillSubtasks(Task* subNode, QTreeWidgetItem* pItem) {
    if (!subNode) return;
    fillSubtasks(subNode->left, pItem);

    QTreeWidgetItem* s = new QTreeWidgetItem(pItem);
    s->setText(0, subNode->title);
    s->setText(1, QString::number(subNode->priority));
    s->setText(2, subNode->date.toString("yyyy-MM-dd"));
    s->setBackground(1, getPriorityColor(subNode->priority));
    s->setForeground(1, Qt::black);
    s->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    s->setCheckState(3, Qt::Unchecked);

    fillSubtasks(subNode->right, pItem);
}
// Priority color
QColor MainWindow::getPriorityColor(int priority) {
    switch(priority) {
    case 10: return QColor(255, 60, 60);   case 9:  return QColor(255, 100, 80);
    case 8:  return QColor(255, 160, 80);  case 7:  return QColor(255, 200, 80);
    case 6:  return QColor(255, 240, 100); case 5:  return QColor(200, 255, 100);
    case 4:  return QColor(140, 255, 100); case 3:  return QColor(100, 255, 180);
    case 2:  return QColor(100, 230, 255); case 1:  return QColor(100, 180, 255);
    default: return Qt::white;
    }
}
