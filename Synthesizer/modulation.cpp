#include "modulation.h"
#include <math.h>
#include <stdlib.h>
#include "QDebug"

Modulation::Modulation() : modulationIndex(1) {

}

void Modulation::setCarrierId(int id){
    carrierId = id;
}

void Modulation::setModulatorId(int id){
    modulatorId = id;
}

void Modulation::setModulationIndex(float index){
    modulationIndex = index;
}

int Modulation::getCarrierId(){
    return carrierId;
}

int Modulation::getModulatorId(){
    return modulatorId;
}

float Modulation::getModulationIndex(){
    return modulationIndex;
}
