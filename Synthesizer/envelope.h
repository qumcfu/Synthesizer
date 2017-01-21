#ifndef ENVELOPE_H
#define ENVELOPE_H

class Envelope
{
public:
    enum State{Off, Attack, Decay, Sustain, Release};
    Envelope();
    void setSampleRate(float sampleRate);
    float process(float input);
    void on();
    void off();
    bool isOn();

    void setReleaseSeconds(float seconds);
    void setAttackSeconds(float seconds);
    void setDecaySeconds(float seconds);
    void setSustain_dB(float sustain_dB);
    void setState(State state);
    int updateEnvelopeArray(int* coords);

    float getAttackSeconds();
    float getDecaySeconds();
    float getReleaseSeconds();
    float getSustain_dB();

private:
    float sampleRate;
    float gain;
    State state;

    float gainChange;
    float releaseSeconds;
    float attackSeconds;
    float decaySeconds;
    float sustain_dB;
    float sustainGain;

    float passPercent;
    float passChange;
};

#endif // ENVELOPE_H
