#ifndef SPEAKTHREAD_H
#define SPEAKTHREAD_H
#include <QThread>
#include</usr/include/espeak/speak_lib.h>
#include <QStringList>
#include <QDebug>

class SpeakThread: public QThread
{
    Q_OBJECT;
private:
    QStringList *text;
    int *speed,*volume;
    int *pos;
    bool *paused;
public:
    SpeakThread(QStringList *list, int *mn_speed,int *mn_volume, bool *mn_pause, int *mn_pos,QObject *parent);
    void run();
    bool getpaused();
signals:
    void scroll();
    void book_finished();

};
int SynthCallback(short *wav, int numsamples, espeak_EVENT *events);

#endif // SPEAKTHREAD_H
