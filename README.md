# To-Do List Application (Binary Search Tree)

A desktop to-do list application built in C++ using a custom Binary Search Tree as the core data structure. Developed as part of a Data Structures course at Misr International University.

## What It Does

- Add tasks with a title, priority level (1–10), and due date
- Add subtasks nested under any main task
- Sort all tasks dynamically by **priority** or **due date**
- Search for any task by name
- Mark tasks as done to remove them
- Export your task list to a CSV file and import it back later

## What Makes It Interesting

The core data structure is a **custom-built BST** — no STL containers used for task storage. Each node in the BST can also hold its own subtask BST, creating a nested tree structure.

When the sort mode is switched (priority ↔ date), the entire tree is **rebuilt** by traversing and reinserting every node under the new sort key — this was implemented manually rather than relying on any library sorting.

The GUI layer (built with Qt) is kept completely separate from the BST logic, which lives entirely in `bst.h`.

## Key Concepts Used

- **Binary Search Tree** — custom implementation with insert, delete, search, and in-order traversal
- **Nested trees** — each task node contains a secondary BST root for subtasks
- **Tree rebuilding** — full traversal and reinsertion to re-sort by a different key
- **File I/O** — CSV export and import using Qt file streams (`QFile`, `QTextStream`)
- **Separation of concerns** — all data structure logic in `bst.h`, all UI logic in `mainwindow.cpp`

## Tech Stack

- C++
- Qt Framework (Qt Creator)
- Custom BST (no STL for core logic)

## Project Structure

```
├── bst.h              # All data structure logic — BST class, Task struct, insert/delete/search/rebuild
├── mainwindow.h       # GUI class declaration
├── mainwindow.cpp     # GUI implementation — connects Qt widgets to BST logic
└── main.cpp           # Entry point
```
