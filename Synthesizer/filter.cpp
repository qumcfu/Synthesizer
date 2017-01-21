#include <math.h>
#include <stdlib.h>
#include "filter.h"

const float pi = 3.1415926f;

Filter::Filter() {
    active = false;
    setCutoff(2000.0f);
    setFilterType(None);
}

void Filter::setFilterType(Type type){
    filterType = type;
    if (type != None){
        active = true;
    } else {
        active = false;
    }
}

void Filter::setCutoff(float cutoff){
    frequency = cutoff;
    recalculate();
}

void Filter::setSampleRate(float sampleRate){
    this->sampleRate = sampleRate;
    recalculate();
}

void Filter::setRipple_dB(float ripple_dB){
    this->ripple_dB = ripple_dB;
    recalculate();
}

Filter::Type Filter::getFilterType(){
    return filterType;
}

float Filter::getCutoff(){
    return frequency;
}

bool Filter::isActive(){
    return active;
}

void Filter::recalculate(){
    // First calculate the prewarped digital frequency:

    k = tan(pi * frequency / sampleRate);

    // Now we calc some Coefficients:

    sg = sinh(ripple_dB);
    cg = cosh(ripple_dB);
    cg *= cg;

    coeff[0] = 1 / (cg - 0.85355339059327376220042218105097);
    coeff[1] = k * coeff[0] * sg * 1.847759065022573512256366378792;
    coeff[2] = 1 / (cg - 0.14644660940672623779957781894758);
    coeff[3] = k * coeff[2] * sg * 0.76536686473017954345691996806;

    k *= k; // (just to optimize it a little bit)

    // Calculate the first biquad:

    a0 = 1 / (coeff[1] + k + coeff[0]);
    a1 = 2 * (coeff[0] - k) * a0;
    a2 = (coeff[1] - k - coeff[0]) * a0;
    b0 = a0 * k;
    b1 = 2 * b0;
    b2 = b0;

    // Calculate the second biquad:

    a3 = 1 / (coeff[3] + k + coeff[2]);
    a4 = 2 * (coeff[2] - k) * a3;
    a5 = (coeff[3] - k - coeff[2]) * a3;
    b3 = a3 * k;
    b4 = 2 * b3;
    b5 = b3;
}

float Filter::process(float input){
    // Then calculate the output as follows:

    if (filterType == None){
        return input;
    }

    double stage1 = b0 * input + state0;
    state0 = b1 * input + a1 * stage1 + state1;
    state1 = b2 * input + a2 * stage1;

    double output = b3 * stage1 + state2;
    state2 = b4 * stage1 + a4 * output + state3;
    state3 = b5 * stage1 + a5 * output;
    return output;
}
