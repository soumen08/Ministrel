#include "speakthread.h"

SpeakThread::SpeakThread(QStringList *list, int *mn_speed, int *mn_volume, bool *mn_pause, int *mn_pos,QObject *parent):QThread(parent)
{
    /*!
      Sets the values of the local parameters to the values received in the constructor.
      */
    text=list;
    speed=mn_speed;
    volume=mn_volume;
    paused=mn_pause;
    pos=mn_pos;
}
void SpeakThread::run()
{
    /*!
      Sets various parameters, extracts the sentence, and converts it to the required format.
      Calls Synth and Synchronize.
      increments pos.
      */
    QString sentence;
    const char *txt;
        while(*pos<(text->length()))
        {
            if (*paused == 0)
            {
                emit scroll();
                sentence=text->at(*pos);
                txt=sentence.toStdString().c_str();
                espeak_SetParameter(espeakRATE,*speed,0);
                espeak_SetParameter(espeakVOLUME,*volume,0);
                espeak_Synth(txt,1000,0,POS_CHARACTER,0,1,NULL,this);
                espeak_Synchronize();
                *pos=*pos+1;
            }
        }
        emit book_finished();

}
