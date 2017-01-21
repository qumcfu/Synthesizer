#ifndef OSCILLATORSOURCE_H
#define OSCILLATORSOURCE_H

#include <vector>

#include "audiosource.h"
#include "oscillator.h"
#include "envelope.h"
#include "modulation.h"
#include "addition.h"
#include "filter.h"

class OscillatorSource: public AudioSource{
public:
    OscillatorSource();
    virtual const QAudioFormat& format() const;
    virtual qint64 read(float** buffer, qint64 numFrames);
    virtual void start();
    virtual void stop();
    enum SynthesisType {Simple, FreqMod, Add};
    void setSelectedOscillator(int oscillatorIndex, int oscillatorType);
    int getOscillatorType (int oscillatorIndex);
    void setFrequency(int oscillatorIndex, float frequency);
    float getFrequency(int oscillatorIndex);
    void setNote(int oscillatorIndex, int noteNumber);
    void setGain(int oscillatorIndex, float decibel);
    float getGain (int oscillatorIndex);
    void setPollution(int oscillatorIndex, float value);
    float getPollution(int oscillatorIndex);
    void noteOn(int oscillatorIndex);
    void noteOff(int oscillatorIndex, int id);
    bool isOscillatorOn(int index);
    void setAttackSeconds(int oscillatorIndex, float value);
    void setDecaySeconds(int oscillatorIndex, float value);
    void setReleaseSeconds(int oscillatorIndex, float value);
    void setSustain_dB(int oscillatorIndex, float value);
    float getAttackSeconds(int oscillatorIndex);
    float getDecaySeconds(int oscillatorIndex);
    float getReleaseSeconds(int oscillatorIndex);
    float getSustain_dB(int oscillatorIndex);
    float createSample(int oscillatorIndex);
    void writeIconSamples(float* samplesArray, int oscillatorIndex, int sampleCount, bool normalized, int offset);
    void setActiveOscillatorId(int oscillatorIndex);
    void setSynthesisType(SynthesisType type);
    SynthesisType getSynthesisType();
    void addModulation(int carrierId, int modulatorId);
    void activateModulation();
    void setModulationIndex(float index);
    float getModulatedValue(Modulation modulation);
    float getModulatedValue(Modulation modulation, int oodID);
    int getCarrierId();
    bool isPartOfModulation(int id);
    void deleteModulation();
    void addAddition(int* oscillatorIds, int oscillatorCount);
    void activateAddition();
    int copyAdditionIdsToArray(int* oscillatorIds);
    void setAdditionMultiplier(float vm);
    float getAddedValue(Addition addition);
    bool isPartOfAddition(int id);
    void deleteAddition();
    void setAdditionInterval(int id, int interval);
    int getAdditionInterval(int id);
    int oscillatorCount;
    bool isOscillatorParticipating(int id);
    int updateEnvelopeArray(int oscillatorIndex, int* coords);
    void writeToRecentSamples(float sample);
    void copyRecentSamplesToArray(float* sampleArray);
    void copyOscillatorSettingsToOOD(int oscillator, int ood);
    void copyFrequencyToOOD(int fromOOD, int toOOD);
    int getActiveOscCount();
    int getActiveOODCount();
    int getNoteId(int oscillatorId);
    int getOODNoteId(int oodId);
    float noteIdToFrequency(int id);
    int frequencyToNoteId(float freq);
    void setAdditionGainModifierStep(float step);
    void setFilterType(int oscillatorIndex, Filter::Type type);
    void setFilterCutoff(int oscillatorIndex, float cutoff);
    Filter::Type getFilterType(int oscillatorIndex);
    float getFilterCutoff(int oscillatorIndex);

private:

private:
    QAudioFormat audioFormat;
    Oscillator *oscillators;
    Oscillator *oods;
    Envelope* envelopes;
    Envelope* eods;
    Filter* filters;
    int activeOscillator;
    SynthesisType synthesisType;
    Modulation *modulation;
    Addition *addition;
    bool* participatingOscillators;
    int recentSampleCount;
    int skipSamples;
    int skipSampleCount;
    float recentSamples[200];
    int activeOscillatorCount;
    int activeOODCount;
    int oodCount;
    float oodWeights[15];
    float sampleHistory[20];
    int sampleHistorySize;
    float additionGainModifierStep;

};
#endif // OSCILLATORSOURCE_H
