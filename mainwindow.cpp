#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    /*!
      Sets title of MainWindow as Ministrel.
      Hides the find_box.
      Sets text on the buttons, and the labels (center them).
      Makes sure only plaintext is shown.
      Connects the signals from buttons and sliders to the slots.
      Connects a QShortcut to find() slot.
      Connects the Esc button to find_quit() slot.
      Connects another QShortcut to the new_instance() slot.
      Connects cursor position changed from textedit to pos update.
      Connects F3 to find_next.
      Connects text-changed from the find_box to find_next.
      Gets docdir (directory for file open dialog)
      Resets the volume and speed from QSettings. Also sets maximum and Minimum Values.
      espeak_Initialize.
      Sets text in the textedit. Also sets ensurecursorvisible (for automatic scrolling)
      Assigns the textedit document to cursor.
      Turns off finding mode.
      Connects cursor_position_changed from textedit to pos_update so that pos can be updated.
      Sets ptr to NULL. Also sets paused to true, no_pos_update to false, and file mode to false.
      */
    ui->setupUi(this);
    this->setWindowTitle("Ministrel");
    ui->find_box->setVisible(false);
    ui->open_button->setText("Open");
    ui->pause_button->setText("Read");
    ui->quit_button->setText("Quit");
    ui->speed_label->setText("Speed");
    ui->speed_label->setAlignment(Qt::AlignHCenter);
    ui->volume_label->setText("Volume");
    ui->volume_label->setAlignment(Qt::AlignHCenter);
    ui->textEdit->setAcceptRichText(false);

    connect(ui->open_button,SIGNAL(clicked()),this,SLOT(open()));
    connect(ui->pause_button,SIGNAL(clicked()),this,SLOT(pause()));
    connect(ui->quit_button,SIGNAL(clicked()),this,SLOT(quit()));
    connect(ui->speed_slider,SIGNAL(valueChanged(int)),this,SLOT(change_speed(int)));
    connect(ui->volume_slider,SIGNAL(valueChanged(int)),this,SLOT(change_volume(int)));

    shortcut_new = new QShortcut(this);
    shortcut_new->setKey(QKeySequence("Ctrl+N"));
    connect(shortcut_new,SIGNAL(activated()),this,SLOT(new_instance()));

    shortcut_find = new QShortcut(this);
    shortcut_find->setKey(QKeySequence("Ctrl+F"));
    connect(shortcut_find,SIGNAL(activated()),this,SLOT(find_begin()));

    shortcut_quit_find = new QShortcut(this);
    shortcut_quit_find->setKey(QKeySequence(Qt::Key_Escape));
    connect(shortcut_quit_find,SIGNAL(activated()),this,SLOT(find_quit()));

    shortcut_find_next = new QShortcut(this);
    shortcut_find_next->setKey(QKeySequence(Qt::Key_F3));
    connect(shortcut_find_next,SIGNAL(activated()),this,SLOT(find_next()));

    connect(ui->find_box,SIGNAL(textChanged(QString)),this,SLOT(find(QString)));

    connect(ui->textEdit,SIGNAL(textChanged()),SLOT(update_list()));

    docdir=settings.value("docdir","/home/").toString();
    ui->speed_slider->setMinimum(80);
    ui->speed_slider->setMaximum(390);
    ui->volume_slider->setMinimum(0);
    ui->volume_slider->setMaximum(200);
    speed=settings.value("speed",170).toInt();
    volume=settings.value("volume",100).toInt();
    ui->speed_slider->setValue(speed);
    ui->volume_slider->setValue(volume);

    espeak_Initialize(AUDIO_OUTPUT_PLAYBACK,500,NULL,1);

    fileopened=false; //this is here to ensure that the next line indeed sets text_list (via list_upd)

    ui->textEdit->setText("Open a File or Paste text to read.");

    connect(ui->textEdit,SIGNAL(cursorPositionChanged()),this,SLOT(pos_update()));

    ui->textEdit->ensureCursorVisible();

    ptr=NULL;
    paused=true;no_pos_upd=false;
    cur_pos=0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
    /*!
      Gets a filehandle checks for null on filehandler.
      Updates docdir.
      Reads all the text. Dumps it to the text edit. Splits using '.'. Appends a '.' to all the sentences.
      Retreives the position from settings(defaults to 0). If the file has been read before, reads from 3 sentences back.
      Calculates a char position adding lengths of indivijual sentences.
      Puts the cursor there.
      */
    filename=QFileDialog::getOpenFileName(this,"Choose a file",docdir,"*.txt");
    if(filename.length() != 0)
    {
        QFile file(filename);
        file.open(QIODevice::ReadOnly);

        QFileInfo inf(filename);
        docdir=inf.absoluteDir().absolutePath();
        settings.setValue("docdir",docdir);

        QByteArray arr=file.readAll();
        fileopened=true;
        no_pos_upd=true;
        ui->textEdit->setText(QString(arr));
        text_list=QString(arr).split('.');

        for(int i=0;i<text_list.length();i++)
            text_list[i].append('.');
        QStringList filenamelist=filename.split('/');
        filename=filenamelist[filenamelist.length() - 1];
        pos=settings.value(filename,0).toInt();
        if(pos>5)
            pos-=3;

        cursor=QTextCursor(ui->textEdit->document());
        cur_pos=0;
        for(int i=0;i<pos;i++)
            cur_pos=cur_pos+text_list[i].length();
        cursor.setPosition(cur_pos);
        no_pos_upd=true;
        ui->textEdit->setTextCursor(cursor);
        ui->open_button->setDisabled(true);
    }
}

void MainWindow::update_list()
{
    /*!
      Used to get the text from the text_edit into the text_list variable.
      */
    if(!fileopened)
    {
        no_pos_upd=true;
        QString text=ui->textEdit->toPlainText();
        text_list=text.split('.');
        for(int i=0;i<text_list.length();i++)
            text_list[i].append('.');
    }
    pos=0;cur_pos=0;

}

void MainWindow::pos_update()
{
    /*!
      Gets the new cursor position. Finds the sentence it belongs to, sets pos right before it so we can start reading from here.
      */
    if(!fileopened)
        return;
    if(!no_pos_upd)
    {
        qDebug()<<"pos update called";
        cursor = ui->textEdit->textCursor();
        cur_pos=0;
        pos=0;
        while(cur_pos < cursor.position())
        {
            cur_pos=cur_pos+text_list.at(pos).length();
            pos++;
        }
        if(pos > 0)
        {
            pos--;
            cur_pos=cur_pos-text_list[pos].length();
        }
        cursor.setPosition(cur_pos);
        no_pos_upd=true;
        ui->textEdit->setTextCursor(cursor);
    }
    else
        no_pos_upd=false;
}

void MainWindow::pause()
{
    /*!
      Checks if a speakthread has been setup. If yes, then filps pause.
      Else,
      Sets up a new thread, and passes the reference of text_list to it along with speed, volume and other parameters.
      Connects the scroll signal from the thread to the scroll slot.
      Connects the book_finished signal from the thread to its slot.
      Starts the thread.
      Updates the button text.
      */
    if(ptr != NULL)
        paused = !paused;
    else
    {
        paused=!paused;
        ptr = new SpeakThread(&text_list,&speed,&volume,&paused,&pos,this);
        connect(ptr,SIGNAL(scroll()),this,SLOT(scroll()));
        connect(ptr,SIGNAL(book_finished()),this,SLOT(book_finished()));
        ptr->start();
    }
    if(paused)
        ui->pause_button->setText("Read");
    else
        ui->pause_button->setText("Pause");
}

void MainWindow::book_finished()
{
    /*!
      Removes the settings for this book.
      Tries to get back to the initial state of the application.
      */
    if(fileopened)
    {
        settings.remove(filename);
    }
    paused=true;
    ui->pause_button->setText("Read");
    pos=0;
    fileopened=false;
    ui->open_button->setDisabled(false);
    ptr=NULL;
    cur_pos=0;
    settings.setValue("volume",volume);
    settings.setValue("speed",speed);
}

void MainWindow::scroll()
{
    /*!
      Moves the cursor to the next sentence. This automatically scrolls the text_edit.
      */
    no_pos_upd=true;
    cur_pos=cur_pos+text_list[pos].length();
    cursor.setPosition(cur_pos);
    ui->textEdit->setTextCursor(cursor);
}

void MainWindow::quit()
{
    /*!
      Writes back speed and volume.
      If a file was opened, also writes back the corresponding pos.
      Quits the app.
      */
    settings.setValue("volume",volume);
    settings.setValue("speed",speed);
    if(fileopened)
        settings.setValue(filename,pos);
    qApp->quit();
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    /*!
      The quit handler. The event is then accepted.
      */
    settings.setValue("volume",volume);
    settings.setValue("speed",speed);
    if(fileopened)
        settings.setValue(filename,pos);
    qApp->quit();
    event->accept();
}

void MainWindow::change_speed(int speed_local)
{
    /*!
      updates the speed variable.
      */
    speed=speed_local;
}

void MainWindow::change_volume(int volume_local)
{
    /*!
      updates the volume variable.
      */
    volume=volume_local;

}

void MainWindow::find_begin()
{
    /*!
      Sets finding to true.
      Sets the find_box to visible and brings it to focus
      */
    paused=true;
    ui->pause_button->setText("Read");
    ui->find_box->setVisible(true);
    ui->find_box->setFocus();
}

void MainWindow::find(QString str)
{
    /*!
      Calls find on the textedit with substring read above.
      cur_pos is updated so that find_next can begin from after the first match(or find_quit can get the correct pos)
      Selects the match (so as to highlight it)
      Sets no_pos_update to true before cursor modifications so as to prevent calls to pos_update.
      Set the cursor of the textedit so as to scroll the textedit to the first match.
      */
    cursor=ui->textEdit->document()->find(str);
    cur_pos=cursor.position();
    cursor.movePosition(QTextCursor::PreviousCharacter);
    cursor.select(QTextCursor::WordUnderCursor);
    no_pos_upd=true;
    ui->textEdit->setTextCursor(cursor);
}

void MainWindow::find_next()
{
    /*!
      Moves to the next instance of the text in the find_box.
      Reads the text from the find box.
      Finds the next match from after the prev match.
      Updates the cur_pos to the match, so that the next match can be found(if F3 is pressed later)
      Highlights(selects) the match.
      Sets no_pos_upd to true so that it does not lead to cascading calls of pos_update.
      Set the cursor, so as to scroll the textedit.
     */
    QString str=ui->find_box->text();
    cursor = ui->textEdit->document()->find(str,cur_pos);
    cur_pos=cursor.position();
    cursor.movePosition(QTextCursor::PreviousCharacter);
    cursor.select(QTextCursor::WordUnderCursor);
    no_pos_upd=true;
    ui->textEdit->setTextCursor(cursor);
}

void MainWindow::find_quit()
{
    /*!
      cur_pos at this point contains the cursor value for the match.
      Sets the cursor to cur_pos, and calls pos_update so that we can read on from the beginning of this sentence.
      Hides find_box.
      */
    cursor=QTextCursor(ui->textEdit->document());
    cursor.setPosition(cur_pos);
    ui->textEdit->setTextCursor(cursor);
    this->pos_update();
    ui->find_box->setVisible(false);
}

void MainWindow::new_instance()
{
    /*!
      Uses system to make a new instance.
      */
    system("Ministrel");
}
