#include <QDebug>
#include "midioutput.h"
#include "midiinput.h"

using namespace drumstick::rt;

int main(int argc, char *argv[])
{
    MIDIOutput midiOutput;

    QStringList outputConnections = midiOutput.connections(true);
    qDebug() << "MIDI Output Connections:";
    for (int i = 0; i < outputConnections.size(); ++i){
        qDebug() << outputConnections.at(i);
    }

    MIDIInput midiInput;
    QStringList inputConnections = midiInput.connections(true);
    qDebug() << "MIDI Input Connections:";
    for (int i = 0; i < inputConnections.size(); ++i){
        qDebug() << inputConnections.at(i);
    }

    return 0;
}
