#ifndef BST_H
#define BST_H

#include <QString>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <iostream>

using namespace std;

// --- 0. DATA STRUCTURE ---
struct Task {
    QString title;
    int priority;
    QDate date;
    Task *left, *right;
    Task *subtasksRoot;

    Task(QString t, int p, QDate d) : title(t), priority(p), date(d), left(nullptr), right(nullptr), subtasksRoot(nullptr) {}
};

// --- 1. BST CLASS ---
class BST {
public:
    Task* root;

    BST() : root(nullptr) {}
    ~BST() { clear(root); }

    void clear() { clear(root); root = nullptr; }
    void clear(Task* n) {
        if(!n) return;
        clear(n->left); clear(n->right); clear(n->subtasksRoot);
        delete n;
    }

    // --- INSERTION ---
    void insert(Task* newTask, int mode) { root = insertRec(root, newTask, mode); }
    void addSubtask(Task* parent, Task* sub, int mode) {
        if(parent) parent->subtasksRoot = insertRec(parent->subtasksRoot, sub, mode);
    }

    Task* insertRec(Task* n, Task* t, int mode) {
        if (!n) return t;
        bool goLeft = (mode == 0) ? (t->priority < n->priority) : (t->date < n->date);
        if (goLeft) n->left = insertRec(n->left, t, mode);
        else        n->right = insertRec(n->right, t, mode);
        return n;
    }

    // --- DELETION (Fixed Traverse-All Version) ---
    void removeMain(QString title, int mode) { root = deleteRec(root, title, mode); }
    void removeSubtask(Task* parent, QString title, int mode) {
        if(parent) parent->subtasksRoot = deleteRec(parent->subtasksRoot, title, mode);
    }

    Task* deleteRec(Task* n, QString title, int mode) {
        if(!n) return nullptr;

        // 1. Check if THIS is the node to delete
        if (n->title == title) {
            if(!n->left) { Task* temp = n->right; delete n; return temp; }
            else if(!n->right) { Task* temp = n->left; delete n; return temp; }

            // Two children case
            Task* temp = minValueNode(n->right);
            n->title = temp->title; n->priority = temp->priority; n->date = temp->date; n->subtasksRoot = temp->subtasksRoot;
            temp->subtasksRoot = nullptr; // Detach moved subtasks from victim
            n->right = deleteRec(n->right, temp->title, mode);
        }
        else {
            // 2. Search BOTH sides (Because Sort Key != Search Key)
            n->left = deleteRec(n->left, title, mode);
            n->right = deleteRec(n->right, title, mode);
        }
        return n;
    }

    Task* minValueNode(Task* n) {
        Task* current = n;
        while (current && current->left != nullptr) current = current->left;
        return current;
    }

    // --- SEARCHING & REBUILDING ---
    Task* find(QString title) { return findRec(root, title); }
    Task* findRec(Task* n, QString t) {
        if(!n) return nullptr;
        if(n->title.compare(t, Qt::CaseInsensitive) == 0) return n;
        Task* r = findRec(n->left, t); if(r) return r;
        r = findRec(n->right, t); if(r) return r;
        return findRec(n->subtasksRoot, t);
    }

    void rebuild(int mode) {
        if(!root) return;
        BST tempTree; rebuildRec(root, tempTree, mode);
        this->root = tempTree.root; tempTree.root = nullptr;
    }
    void rebuildRec(Task* n, BST& newTree, int mode) {
        if(!n) return;
        Task* l = n->left; Task* r = n->right;
        n->left = nullptr; n->right = nullptr;
        newTree.insert(n, mode);
        rebuildRec(l, newTree, mode); rebuildRec(r, newTree, mode);
        if(n->subtasksRoot) {
            BST subTemp; rebuildRec(n->subtasksRoot, subTemp, mode);
            n->subtasksRoot = subTemp.root; subTemp.root = nullptr;
        }
    }

    // --- COUNTERS ---
    int countMain() { return countRec(root); }
    int countSub() { return countAllSub(root); }
    int countRec(Task* n) { return n ? 1 + countRec(n->left) + countRec(n->right) : 0; }
    int countNested(Task* n) { return n ? 1 + countNested(n->left) + countNested(n->right) : 0; }
    int countAllSub(Task* n) { return n ? countNested(n->subtasksRoot) + countAllSub(n->left) + countAllSub(n->right) : 0; }

    // --- FILE I/O (Export/Import) ---
    void exportToCSV(QString filename) {
        QFile f(filename);
        if(f.open(QIODevice::WriteOnly)) {
            QTextStream out(&f);
            saveNodes(root, out);
        }
    }

    void importFromCSV(QString filename, int mode) {
        QFile f(filename);
        if(!f.open(QIODevice::ReadOnly)) return;
        clear(); // Wipe current tree
        QTextStream in(&f);
        Task* lastMain = nullptr;
        while(!in.atEnd()){
            QString line = in.readLine();
            QStringList r = line.split(",");
            if(r.size() < 4) continue;

            Task* t = new Task(r[1], r[2].toInt(), QDate::fromString(r[3], "yyyy-MM-dd"));

            if(r[0] == "S" && lastMain) addSubtask(lastMain, t, mode);
            else {
                insert(t, mode);
                lastMain = t;
            }
        }
    }

private:
    // Helper to save Main Nodes
    void saveNodes(Task* n, QTextStream& o) {
        if(!n) return;
        saveNodes(n->left, o);
        o << "M," << n->title << "," << n->priority << "," << n->date.toString("yyyy-MM-dd") << "\n";
        saveSubNodes(n->subtasksRoot, o); // Save subtasks immediately after parent
        saveNodes(n->right, o);
    }
    // Helper to save Sub Nodes
    void saveSubNodes(Task* s, QTextStream& o) {
        if(!s) return;
        saveSubNodes(s->left, o);
        o << "S," << s->title << "," << s->priority << "," << s->date.toString("yyyy-MM-dd") << "\n";
        saveSubNodes(s->right, o);
    }

    // --- VISUALIZATION HELPER (The Logic You Wanted) ---
public:
    void printConsole(int mode) {
        cout << "\n\n================ VISUAL TREE (" << (mode == 0 ? "Priority" : "Date") << ") ================" << endl;
        if (!root) { cout << "(Empty Tree)" << endl; return; }
        printSubtree(root, "", "ROOT", mode);
        cout << "============================================================" << endl;
        cout << flush;
    }

private:
    void printSubtree(Task* n, QString prefix, QString type, int mode) {
        if (!n) return;

        cout << qPrintable(prefix);

        if (type == "ROOT")      cout << "";
        else if (type == "L")    cout << "+--L-- ";
        else if (type == "R")    cout << "+--R-- ";
        else if (type == "S")    cout << "+--S-- ";

        QString meta = (mode == 0) ? QString("P:%1").arg(n->priority) : n->date.toString("yyyy-MM-dd");
        QString label = QString("[%1] %2").arg(meta, n->title);
        cout << qPrintable(label) << endl;

        QString childPrefix = prefix;
        if (type != "ROOT") childPrefix += "|      ";

        // Print Subtasks First
        if (n->subtasksRoot) {
            printSubtree(n->subtasksRoot, childPrefix, "S", mode);
        }

        // Print Children
        if (n->left || n->right) {
            if (n->left) printSubtree(n->left, childPrefix, "L", mode);
            else         cout << qPrintable(childPrefix) << "+--L-- (Empty)" << endl;

            if (n->right) printSubtree(n->right, childPrefix, "R", mode);
            else          cout << qPrintable(childPrefix) << "+--R-- (Empty)" << endl;
        }
    }
};

#endif // BST_H
