#include <math.h>
#include <stdlib.h>
#include "oscillator.h"
#include <QDebug>

const float pi = 3.1415926f;

Oscillator::Oscillator () : sampleRate (0), index (0), frequency (0), type (Sine), gain (1), pollution (0) {
    additionInterval = 0;
}

void Oscillator::initialize (float sampleRate){
    this->sampleRate = sampleRate;
}
void Oscillator::setFrequency (float setFrequency){
    frequency = setFrequency;
}

float Oscillator::getFrequency(){
    return frequency;
}

int Oscillator::getNoteId(){
    return int((69.0 + 12.0 * log (frequency / 440.0) / log (2.0)) + 0.5);
}

void Oscillator::setType (Type type){
    this->type = type;
}
int Oscillator::getType(){
    return type;
}

void Oscillator::setGain (float gain){
    this->gain = gain;
}

float Oscillator::getGain(){
    return gain;
}

void Oscillator::setPollution(float pollution){
    this->pollution = pollution;
}

float Oscillator::getPollution(){
    return pollution;
}

void Oscillator::setAdditionInterval(int interval){
    additionInterval = interval;
}

int Oscillator::getAdditionInterval(){
    return additionInterval;
}

float Oscillator::getValue (){
    switch (type){
    case Sine:
        return gain * sine ();
    case Saw:
        return gain * saw ();
    case Square:
        return gain * square ();
    case Triangle:
        return gain * triangle ();
    case Noise:
        return gain * noise ();
    }
}

void Oscillator::setIndex(int index){
    this->index = index;
}

float Oscillator::sine (){
    float value = sin (2 * pi * index * frequency / sampleRate);
    if (pollution != 0){
        value = clamp01(value + getRandomPollution());
    }
    index++;
    return value;
}
float Oscillator::saw (){
    float period = sampleRate / frequency;
    float modulo = fmod (index, period);
    index++;
    float value = (modulo / period) * 2 - 1;
    if (pollution != 0){
        value = clamp01(value + getRandomPollution());
    }
    return value;
}
float Oscillator::square (){
    float value = sin (2 * pi * index * frequency / sampleRate);
    index++;
    value = (value > 0)? 1 : -1;
    if (pollution != 0){
        value = clamp01(value + getRandomPollution());
    }
    return value;
}
float Oscillator::triangle (){
    float period = sampleRate / frequency;
    float modulo = fmod(index, period);
    index++;

    float value = (modulo / period) * 4;
    if (value < 2){
        value -= 1;
    } else {
        value = (1 + 2 - value);
    }
    if (pollution != 0){
        value = clamp01(value + getRandomPollution());
    }
    return value;
}
float Oscillator::noise (){
    float random = rand();
    return 2 * random / RAND_MAX - 1;
}

float Oscillator::getRandomPollution(){
    float random = rand();
    return pollution * (2 * random / RAND_MAX - 1);
}

float Oscillator::clamp01(float value){
    return value < -1 ? -1 : value > 1 ? 1 : value;
}

void Oscillator::writeIconSamples (float* samplesArray, int sampleCount, bool normalized, int offset){
    float prevFrequency = frequency;
    int prevIndex = index;
    float prevGain = gain;

    index = 25 + offset;
    if (normalized){
        frequency = 960.0f;
    }
    gain = 1.0f;

    for (int i = 0; i < sampleCount; i++){
        samplesArray[i] = getValue ();
    }

    frequency = prevFrequency;
    index = prevIndex;
    gain = prevGain;
}
