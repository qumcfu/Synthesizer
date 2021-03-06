#include "mainwindow.h"
#include "detection.h"
#include <QApplication>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include "midiinput.h"

using namespace drumstick::rt;

class Memory{
  MainWindow* w;
  Detection *d;
  Memory() {}
  virtual ~Memory() {
      delete m_instance;
  }

public:
  static Memory* instance(){
    if ( !m_instance )
      m_instance = new Memory();

    return m_instance;
  }

  void setMainWindow(MainWindow &w){
    this->w = &w;
  }

  MainWindow* mainWindow(){
      return w;
  }

  void setDetection(Detection &d){
      this->d = &d;
  }

  Detection* detection(){
      return d;
  }

protected:
  static Memory* m_instance;
};

Memory* Memory::m_instance = NULL;

void onMidiNoteOff(const int chan, const int note, const int vel){
//    qDebug() << "note off: " << chan << " " << note << " " << vel;
    Memory::instance()->mainWindow()->releaseMidiNote(note);
}

void onMidiNoteOn(const int chan, const int note, const int vel){
//    qDebug() << "note on: " << chan << " " << note << " " << vel;
    if (vel == 0){
        Memory::instance()->mainWindow()->releaseMidiNote(note);
    } else {
        Memory::instance()->mainWindow()->playMidiNote(note);
    }
}

void onMidiController(const int chan, const int control, const int value){
    qDebug() << "controller: " << chan << " " << control << " " << value;
}

class Thread : public QThread{
private:
    void run(){

//        qDebug()<<"From worker thread: "<<currentThreadId();
        MIDIInput midiInput;


        QStringList inputConnections = midiInput.connections(true);
        qDebug() << "MIDI Input Connections:";
        for (int i = 0; i < inputConnections.size(); ++i){
            qDebug() << inputConnections.at(i);
        }
        midiInput.open("LoopBe Internal MIDI");
        QObject::connect(&midiInput, &MIDIInput::midiNoteOn, onMidiNoteOn);
        QObject::connect(&midiInput, &MIDIInput::midiNoteOff, onMidiNoteOff);
        QObject::connect(&midiInput, &MIDIInput::midiController, onMidiController);

        // wait for any key
        getchar();
    }
};

class DetectionThread : public QThread{
private:
    void run(){
        Memory::instance()->detection()->init();
    }
};

int main(int argc, char *argv[]){
    QApplication a(argc, argv);

    MainWindow w;
    Detection d;
    w.setDetection(&d);
    w.show();


//    qDebug()<<"From main thread: "<<QThread::currentThreadId();

    Thread t;
    DetectionThread dt;
    Memory* m = Memory::instance();
    m->setMainWindow(w);
    m->setDetection(d);
//    QObject::connect(&t, SIGNAL(finished()), &a, SLOT(quit()));

    t.start();
    dt.start();

    return a.exec();
}

