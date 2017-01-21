#include <QDebug>
#include <math.h>
#include "envelope.h"

const float MIN_GAIN_DB = -180;
const float MIN_GAIN = pow(10, MIN_GAIN_DB/20);

inline float dB2gain(float dB){
    return pow(10, dB/20);
}
inline float gain2dB(float gain){
    return 20*log(gain);
}

Envelope::Envelope() : sampleRate(0), gain(0), state(Off), releaseSeconds(0),
                        attackSeconds(0), decaySeconds(0), sustain_dB(MIN_GAIN_DB), sustainGain(MIN_GAIN),
                        passPercent(0.0f), passChange(0.01f){
    setReleaseSeconds(0.0f);
    setAttackSeconds(0.0f);
    setDecaySeconds(0.0f);
    setSustain_dB(0);
}

void Envelope::setSampleRate(float sampleRate){
    this->sampleRate = sampleRate;
}
void Envelope::setReleaseSeconds(float seconds){
    this->releaseSeconds = seconds;
}
void Envelope::setAttackSeconds(float seconds){
    this->attackSeconds = seconds;
}
void Envelope::setDecaySeconds(float seconds){
    this->decaySeconds = seconds + 0.0001f;
}
void Envelope::setSustain_dB(float sustain_dB){
    this->sustainGain = dB2gain(sustain_dB);
    this->sustain_dB = sustain_dB;
}

float Envelope::getAttackSeconds(){
    return attackSeconds;
}
float Envelope::getDecaySeconds(){
    return decaySeconds;
}
float Envelope::getReleaseSeconds(){
    return releaseSeconds;
}
float Envelope::getSustain_dB(){
    return sustain_dB;
}

int Envelope::updateEnvelopeArray(int* coords){
    int maxValue = 18;
    int sustainValue = (int)(maxValue * (1 + sustain_dB / 100.0f));
    float totalDuration = attackSeconds + decaySeconds + 2.0f + releaseSeconds; // 0.3, 0.2, 2.0, 0.5
    int sampleCount = (int)(totalDuration * 10.0f);
    if (sampleCount > 70){
        sampleCount = 70;
    }
    int decayStart = (int)(sampleCount * attackSeconds / totalDuration);
    int sustainStart = (int)(sampleCount * (attackSeconds + decaySeconds) / totalDuration);
    int releaseStart = (int)(sampleCount * (totalDuration - releaseSeconds) / totalDuration);
    int releaseSamples = sampleCount - releaseStart - 1;

    for (int i = 0; i < sampleCount; i++){

        if (i < decayStart){
            coords[i] = (int)(maxValue * (i + 1) / (float)(decayStart + 1.0f));
        } else if (i < sustainStart){
            int difference = maxValue - sustainValue;
            int reverseDecayCount = (i + 1 - sustainStart) * -1;
            coords[i] = (int)(sustainValue + difference * reverseDecayCount / (sampleCount * decaySeconds / totalDuration) + 0.5);
        } else if (i < releaseStart){
            coords[i] = sustainValue;
        } else {
            int releaseCount = (sampleCount - i);
            coords[i] = (int)((float)sustainValue * (float)releaseCount / ((float)releaseSamples + 1.0f));
        }
    }

    return sampleCount;
}

void Envelope::on(){
    setState(Attack);
    passPercent = 0.0f;
}
void Envelope::off(){
    setState(Release);
}
bool Envelope::isOn(){
    return (state != Off && state != Release);
}

void Envelope::setState(State state){
    this->state = state;
    // avoid division by zero
    if (state == Attack && attackSeconds == 0){
        setState(Decay);
    } else if (state == Decay && decaySeconds == 0){
        setState(Sustain);
    } else if (state == Release && releaseSeconds == 0){
//        qDebug() << "NO RELEASE TIME SET";
        setState(Off);
    } else {
        if (state == Off){
//            gain = 0;
//            qDebug() << "OFF";
        }
        if (state == Attack){
            float gainChange_dB = fabs(MIN_GAIN_DB) / (attackSeconds * sampleRate);
            gainChange = dB2gain(gainChange_dB);
            gain = MIN_GAIN;
        }
        if (state == Decay){
            gain = 1;
            float gainChange_dB = fabs(sustain_dB) / (decaySeconds * sampleRate);
            gainChange = 1/dB2gain(gainChange_dB);
        }
        if (state == Release){
            float gainChange_dB = fabs(sustain_dB - MIN_GAIN_DB) / (sampleRate * releaseSeconds);
            gainChange =  1/dB2gain(gainChange_dB);
        }
        if (state == Sustain){
            gain = sustainGain;
            gainChange = 1;
        }
    }
}

float Envelope::process(float input){

    if (state != Release && state != Off){
        if (passPercent < 1.0f){
            passPercent += passChange;
        }
    } else if (state == Off){
        if (passPercent > 0.0f){
            passPercent -= passChange;
        } else {
            gain = 0;
        }
    }

    if (state == Attack && gain >= 1){
        setState(Decay);
    }
    if (state == Decay && gain <= sustainGain){
        setState(Sustain);
    }
    if (state == Release && gain <= MIN_GAIN){
        setState(Off);
    }
    gain *= gainChange;
    return gain * input * passPercent;
}
