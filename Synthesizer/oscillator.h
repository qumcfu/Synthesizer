#ifndef OSCILLATOR_H
#define OSCILLATOR_H


class Oscillator {
public:
    Oscillator();
    enum Type {Sine, Triangle, Square, Saw, Noise};
    void initialize (float sampleRate);
    void setFrequency (float frequency);
    float getFrequency ();
    int getNoteId ();
    void setType (Type type);
    int getType ();
    void setGain (float gain);
    float getGain ();
    float getValue ();
    float getIconSampleValue();
    void writeIconSamples (float* samplesArray, int sampleCount, bool normalized, int offset);
    void setActive(bool active);
    bool isParticipating();
    void setPollution (float pollution);
    float getPollution ();
    float getRandomPollution();
    float clamp01(float value);
    void setIndex(int index);
    void setAdditionInterval(int inverval);
    int getAdditionInterval();

private:
    float sine();
    float saw();
    float triangle();
    float square();
    float noise();

    int index;
    int sampleIndex;
    float sampleRate;
    float frequency;
    float gain;
    float pollution;
    int additionInterval;
    Type type;
};

#endif // OSCILLATOR_H
