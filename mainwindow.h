#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "speakthread.h"
#include <QFileDialog>
#include <QStringList>
#include <QTextCursor>
#include <QDebug>
#include <QShortcut>
#include <QTime>
#include <QCloseEvent>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QShortcut *shortcut_new;
    QShortcut *shortcut_find;
    QShortcut *shortcut_quit_find;
    QShortcut *shortcut_find_next;
    QSettings settings;
    int speed,volume;
    QString docdir;
    SpeakThread *ptr;
    QString filename;
    bool fileopened;
    QStringList text_list;
    int pos; //sentence index from which we have to read.
    int cur_pos;
    QTextCursor cursor;
    bool paused;
    bool no_pos_upd;
public slots:
    void open();
    void pos_update();
    void pause();
    void scroll();
    void quit();
    void change_speed(int);
    void change_volume(int);
    void find_begin();
    void find_quit();
    void find(QString);
    void find_next();
    void new_instance();
    void book_finished();
    void update_list();
    void closeEvent(QCloseEvent *);

};

#endif // MAINWINDOW_H
