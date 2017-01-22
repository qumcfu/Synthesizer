#include <QDebug>
#include <QThread>
#include "midiinput.h"

using namespace drumstick::rt;

int main(int argc, char *argv[])
{
    MIDIOutput midiOutput;
    // display list of output connections
    QStringList outputConnections = midiOutput.connections(true);
    qDebug() << "MIDI Output Connections:";
    for (int i = 0; i < outputConnections.size(); ++i){
        qDebug() << outputConnections.at(i);
    }

    // open output connection
    midiOutput.open("Microsoft GS Wavetable Synth");

    // send MIDI Note On
    int channel = 0;
    int note = 60;
    int velocity = 127;
    midiOutput.sendNoteOn(channel, note, velocity);

    // wait 1 second
    QThread::sleep(1);

    // send MIDI Note Off
    midiOutput.sendNoteOff(channel, note, 0);
    return 0;
}
