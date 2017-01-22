#include <QDebug>
#include <QThread>
#include "midiinput.h"

using namespace drumstick::rt;

void onMidiNoteOff(const int chan, const int note, const int vel){
    qDebug() << "note off: " << chan << " " << note << " " << vel;
}

void onMidiNoteOn(const int chan, const int note, const int vel){
    qDebug() << "note on: " << chan << " " << note << " " << vel;
}

void onMidiController(const int chan, const int control, const int value){
    qDebug() << "controller: " << chan << " " << control << " " << value;
}

int connect(int argc, char *argv[])
{
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

    return 0;
}



