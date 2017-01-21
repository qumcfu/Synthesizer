#include <math.h>
#include "oscillatorsource.h"
#include <QDebug>



OscillatorSource::OscillatorSource() : synthesisType(Simple){
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(QAudioFormat::LittleEndian);
    audioFormat.setChannelCount(2);
    audioFormat.setSampleSize(32);
    audioFormat.setSampleType(QAudioFormat::Float);
    audioFormat.setSampleRate(44100);

    oscillatorCount = 4;
    oscillators = new Oscillator[oscillatorCount];
    envelopes = new Envelope[oscillatorCount];
    filters = new Filter[oscillatorCount];

    oodCount = 10;
    oods = new Oscillator[oodCount];
    eods = new Envelope[oodCount];
    activeOscillatorCount = 0;
    activeOODCount = 0;
    for (int i = 0; i < 15; i++){
        oodWeights[i] = 0.0f;
    }

    modulation = new Modulation();
    addition = new Addition();
    activeOscillator = 0;
    addModulation(8, 9);

    recentSampleCount = 200;
    for (int i = 0; i < recentSampleCount; i++){
        recentSamples[i] = 0;
    }
    skipSamples = 0;
    skipSampleCount = 0;

    sampleHistorySize = 20;
    additionGainModifierStep = 0.0f;

}

void OscillatorSource::setSelectedOscillator(int oscillatorIndex, int oscillatorType){
    oscillators[oscillatorIndex].setType((Oscillator::Type)oscillatorType);
}

int OscillatorSource::getOscillatorType(int oscillatorIndex){
    return (int)oscillators[oscillatorIndex].getType();
}

void OscillatorSource::setActiveOscillatorId(int oscillatorIndex){
    activeOscillator = oscillatorIndex;
}

void OscillatorSource::setFrequency(int oscillatorIndex, float frequency){

    if (!envelopes[oscillatorIndex].isOn()){
        oscillators[oscillatorIndex].setFrequency(frequency);
    } else if (oscillatorIndex == activeOscillator && activeOscillatorCount > 0){
        copyOscillatorSettingsToOOD(activeOscillator, activeOODCount);
        oods[activeOODCount].setFrequency(frequency);
    }
}

float OscillatorSource::getFrequency(int oscillatorIndex){
    return oscillators[oscillatorIndex].getFrequency();
}

void OscillatorSource::setNote(int oscillatorIndex, int noteNumber){
    float frequency = 440.0 * pow(2.0, (noteNumber - 69.0)/12.0);
    setFrequency(oscillatorIndex, frequency);
}

float OscillatorSource::noteIdToFrequency(int id){
    return 440.0 * pow(2.0, (id - 69.0)/12.0);
}

int OscillatorSource::frequencyToNoteId(float freq){
    return int((69.0 + 12.0 * log (freq / 440.0) / log (2.0)) + 0.5);
}

void OscillatorSource::setGain(int oscillatorIndex, float decibel){
    float gain = pow(10, decibel/20.f);
    oscillators[oscillatorIndex].setGain(gain);
}

float OscillatorSource::getGain(int oscillatorIndex){
    return oscillators[oscillatorIndex].getGain();
}

void OscillatorSource::setPollution(int oscillatorIndex, float value){
    oscillators[oscillatorIndex].setPollution(value);
}
float OscillatorSource::getPollution(int oscillatorIndex){
    return oscillators[oscillatorIndex].getPollution();
}

void OscillatorSource::noteOn(int oscillatorIndex){
    if (!envelopes[oscillatorIndex].isOn()){
        oscillators[oscillatorIndex].setIndex(0);
        envelopes[oscillatorIndex].on();
        activeOscillatorCount += 1;
    } else if (oscillatorIndex == activeOscillator && activeOscillatorCount > 0){
        oods[activeOODCount].setIndex(0);
        eods[activeOODCount].on();
        activeOODCount += 1;
    }
}
void OscillatorSource::noteOff(int oscillatorIndex, int id){

    if (envelopes[oscillatorIndex].isOn() && activeOODCount <= 0){
        envelopes[oscillatorIndex].off();
        activeOscillatorCount -= 1;
        if (isPartOfModulation(oscillatorIndex)){
            for (int i = 0; i < oscillatorCount; i++){
                if (isPartOfModulation(i)){
                    envelopes[i].off();
                }
            }

        } else if (isPartOfAddition(oscillatorIndex)){
            for (int i = 0; i < oscillatorCount; i++){
                if (isPartOfAddition(i)){
                    envelopes[i].off();
                }
            }
        }
    } else if (envelopes[oscillatorIndex].isOn()){
        bool oodFound = false;

        if (id != -1){
            if (oscillators[oscillatorIndex].getNoteId() == id){
                oscillators[oscillatorIndex].setFrequency(oods[0].getFrequency());
                oodFound = true;
            }

            for (int i = 0; i < activeOODCount; i++){
                if (oods[i].getNoteId() == id){
                    oodFound = true;
                }
                if (oodFound){
                    copyFrequencyToOOD(i + 1, i);
                    if (i >= activeOODCount - 1){
                        eods[i].off();
                        oodWeights[i] = 0.0f;
                    }
                }
            }
        } else {
            qDebug() << "ood not found.";
        }
        activeOODCount -= 1;
    }
}

bool OscillatorSource::isOscillatorOn(int index){
    return envelopes[index].isOn();
}

void OscillatorSource::start(){
    for (int i = 0; i < oscillatorCount; i++){
        oscillators[i].initialize(audioFormat.sampleRate());
        envelopes[i].setSampleRate(audioFormat.sampleRate());
    }
    for (int i = 0; i < oodCount; i++){
        oods[i].initialize(audioFormat.sampleRate());
        eods[i].setSampleRate(audioFormat.sampleRate());
    }
}
const QAudioFormat& OscillatorSource::format() const{
    return audioFormat;
}

float OscillatorSource::createSample(int oscillatorIndex){
    float sample = 0.0f;
    if (synthesisType == Simple){
        sample = envelopes[oscillatorIndex].process(oscillators[oscillatorIndex].getValue());
    } else if (synthesisType == FreqMod){
        sample = getModulatedValue(*modulation);
    } else if (synthesisType == Add){
        sample = getAddedValue(*addition);
    }

    if (activeOODCount > 0){
        float totalWeights = 1.0f;
        float mod = 1.0f;
        float modChange = additionGainModifierStep;
        for (int i = 0; i < activeOODCount; i++){
            if (synthesisType != FreqMod){
                sample += eods[i].process(oods[i].getValue() * oodWeights[i]);
            } else {
                sample += getModulatedValue(*modulation, i) * oodWeights[i];
            }
            if (oodWeights[i] < 1.0f){
                oodWeights[i] += 0.01f;
            }
            totalWeights += oodWeights[i];
            mod -= modChange;
            modChange *= 0.7f;
        }
        sample /= totalWeights * mod;
    }

    if (filters[oscillatorIndex].isActive()){
        sample = filters[oscillatorIndex].process(sample);
    }

    if (skipSampleCount < skipSamples){
        skipSampleCount++;
    } else {
        skipSampleCount = 0;
        writeToRecentSamples(sample);
    }

    float smoothedSample = sample;
    for (int i = 0; i <= sampleHistorySize - 1; i++){
        sampleHistory[i] = sampleHistory[i + 1];
        smoothedSample += sampleHistory[i];
    }
    sampleHistory[sampleHistorySize - 1] = sample;
    smoothedSample /= sampleHistorySize;

    return smoothedSample;
}

void OscillatorSource::writeToRecentSamples(float sample){
    for (int i = 0; i < recentSampleCount - 1; i++){
        recentSamples[i] = recentSamples[i + 1];
    }
    recentSamples[recentSampleCount - 1] = sample;
}

void OscillatorSource::copyRecentSamplesToArray(float *sampleArray){
    for (int i = 0; i < recentSampleCount; i++){
        sampleArray[i] = recentSamples[i];
    }
}

qint64 OscillatorSource::read(float** buffer, qint64 numFrames){
    // get audio data for left channel
    for(int i = 0; i < numFrames; i++){
        buffer[0][i] = createSample(activeOscillator);
    }
    // copy to other channels
    for(int c = 0; c < audioFormat.channelCount(); c++){
        for(int i = 0; i < numFrames; i++){
            buffer[c][i] = buffer[0][i];
        }
    }
    return numFrames;
}

void OscillatorSource::writeIconSamples (float* samplesArray, int oscillatorIndex, int sampleCount, bool normalized, int offset){
    oscillators[oscillatorIndex].writeIconSamples (samplesArray, sampleCount, normalized, offset);
}

void OscillatorSource::setSynthesisType(SynthesisType type){
    synthesisType = type;
}

OscillatorSource::SynthesisType OscillatorSource::getSynthesisType(){
    return synthesisType;
}

void OscillatorSource::addModulation(int carrierId, int modulatorId){
    modulation->setCarrierId(carrierId);
    modulation->setModulatorId(modulatorId);
}

void OscillatorSource::activateModulation(){
    int carrierId = modulation->getCarrierId();
    int modulatorId = modulation->getModulatorId();
    for (int i = 0; i < oscillatorCount; i++){
        if (i == carrierId || i == modulatorId){
            noteOn(i);
        } else {
            noteOff(i, -1);
        }
    }
}

float OscillatorSource::getModulatedValue(Modulation modulation){
    float carrierValue = envelopes[modulation.getCarrierId()].process(oscillators[modulation.getCarrierId()].getValue());
    float modulatorValue = envelopes[modulation.getCarrierId()].process(oscillators[modulation.getModulatorId()].getValue());
    return cos(carrierValue + modulation.getModulationIndex() * cos(modulatorValue));
}

float OscillatorSource::getModulatedValue(Modulation modulation, int oodID){
    float carrierValue = eods[oodID].process(oods[oodID].getValue());
    float modulatorValue = envelopes[modulation.getCarrierId()].process(oscillators[modulation.getModulatorId()].getValue());
    return cos(carrierValue + modulation.getModulationIndex() * cos(modulatorValue));
}

void OscillatorSource::setModulationIndex(float index){
    modulation->setModulationIndex(index);
}

int OscillatorSource::getCarrierId(){
    return modulation->getCarrierId();
}

bool OscillatorSource::isPartOfModulation(int id){
    return (id == modulation->getCarrierId() || id == modulation->getModulatorId());
}

void OscillatorSource::deleteModulation(){
    addModulation(-1, -1);
}

void OscillatorSource::addAddition(int* oscillatorIds, int oscillatorCount){
    addition->setOscillatorIds(oscillatorIds, oscillatorCount);
}

void OscillatorSource::activateAddition(){
    int additionIds[10] = {};
    int oscillatorCount = addition->copyIdsToArray(additionIds);
    for (int i = 0; i < oscillatorCount; i++){
        noteOn(additionIds[i]);
    }
}

int OscillatorSource::copyAdditionIdsToArray(int *oscillatorIds){
    return addition->copyIdsToArray(oscillatorIds);
}

float OscillatorSource::getAddedValue(Addition addition){
    float totalValues = 0.0f;
    int additionIds[10];
    int oscillatorCount = addition.copyIdsToArray(additionIds);
    for (int i = 0; i < oscillatorCount; i++){

        totalValues += envelopes[additionIds[i]].process(oscillators[additionIds[i]].getValue());
    }
    return totalValues / oscillatorCount * addition.getValueMultiplier();
}

void OscillatorSource::setAdditionMultiplier(float vm){
    addition->setValueMultiplier(vm);
}

bool OscillatorSource::isPartOfAddition(int id){
    bool isPart = false;
    int additionIds[10] = {};
    int oscillatorCount = addition->copyIdsToArray(additionIds);
    for (int i = 0; i < oscillatorCount; i++){
        if (id == additionIds[i]){
            isPart = true;
        }
    }
    return isPart;
}

void OscillatorSource::deleteAddition(){
    int* ids[0] = {};
    addAddition(*ids, 0);
}

void OscillatorSource::setAdditionInterval(int id, int interval){
    oscillators[id].setAdditionInterval(interval);
}

int OscillatorSource::getAdditionInterval(int id){
    return oscillators[id].getAdditionInterval();
}

bool OscillatorSource::isOscillatorParticipating(int id){
    bool participating = false;
    if (envelopes[id].isOn()){
        participating = true;
    }
    return participating;
}

void OscillatorSource::setAttackSeconds(int oscillatorIndex, float value){
    envelopes[oscillatorIndex].setAttackSeconds(value);
}

float OscillatorSource::getAttackSeconds(int oscillatorIndex){
    return envelopes[oscillatorIndex].getAttackSeconds();
}

float OscillatorSource::getDecaySeconds(int oscillatorIndex){
    return envelopes[oscillatorIndex].getDecaySeconds();
}

float OscillatorSource::getReleaseSeconds(int oscillatorIndex){
    return envelopes[oscillatorIndex].getReleaseSeconds();
}

float OscillatorSource::getSustain_dB(int oscillatorIndex){
    return envelopes[oscillatorIndex].getSustain_dB();
}

void OscillatorSource::setDecaySeconds(int oscillatorIndex, float value){
    envelopes[oscillatorIndex].setDecaySeconds(value);
}

void OscillatorSource::setReleaseSeconds(int oscillatorIndex, float value){
    envelopes[oscillatorIndex].setReleaseSeconds(value);
}

void OscillatorSource::setSustain_dB(int oscillatorIndex, float value){
    envelopes[oscillatorIndex].setSustain_dB(value);
}

int OscillatorSource::updateEnvelopeArray(int oscillatorIndex, int *coords){
    return envelopes[oscillatorIndex].updateEnvelopeArray(coords);
}

void OscillatorSource::copyOscillatorSettingsToOOD(int oscillator, int ood){
    oods[ood].setType((Oscillator::Type)oscillators[oscillator].getType());
    eods[ood].setAttackSeconds(envelopes[oscillator].getAttackSeconds());
    eods[ood].setDecaySeconds(envelopes[oscillator].getDecaySeconds());
    eods[ood].setReleaseSeconds(envelopes[oscillator].getReleaseSeconds());
    eods[ood].setSustain_dB(envelopes[oscillator].getSustain_dB());
    oods[ood].setPollution(oscillators[oscillator].getPollution());
}

void OscillatorSource::copyFrequencyToOOD(int fromOOD, int toOOD){
    oods[toOOD].setFrequency(oods[fromOOD].getFrequency());
}

int OscillatorSource::getActiveOscCount(){
    return activeOscillatorCount;
}

int OscillatorSource::getActiveOODCount(){
    return activeOODCount;
}

int OscillatorSource::getNoteId(int oscillatorId){
    int id = oscillators[oscillatorId].getNoteId();
    return (id >= 12 && id <= 124) ? id : -1;
}

int OscillatorSource::getOODNoteId(int oodId){
    int id = oods[oodId].getNoteId();
    return (id >= 12 && id <= 124) ? id : -1;
}

void OscillatorSource::setAdditionGainModifierStep(float step){
    additionGainModifierStep = step;
}

void OscillatorSource::setFilterType(int oscillatorIndex, Filter::Type type){
    filters[oscillatorIndex].setFilterType(type);
    filters[oscillatorIndex].setRipple_dB(1);
    filters[oscillatorIndex].setSampleRate(44100);
}

void OscillatorSource::setFilterCutoff(int oscillatorIndex, float cutoff){
    filters[oscillatorIndex].setCutoff(cutoff);
}

Filter::Type OscillatorSource::getFilterType(int oscillatorIndex){
    return filters[oscillatorIndex].getFilterType();
}

float OscillatorSource::getFilterCutoff(int oscillatorIndex){
    return filters[oscillatorIndex].getCutoff();
}

void OscillatorSource::stop(){}
