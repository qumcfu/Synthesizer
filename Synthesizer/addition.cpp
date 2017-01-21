#include "addition.h"

Addition::Addition() : oscillatorCount(0), valueMultiplier(1){

    oscillatorIds = new int[10];
}

void Addition::setOscillatorIds(int *ids, int oscillatorCount){
    for (int i = 0; i < oscillatorCount; i++){
        oscillatorIds[i] = ids[i];
    }
    this->oscillatorCount = oscillatorCount;
}

void Addition::setValueMultiplier(float vm){
    valueMultiplier = vm;
}

float Addition::getValueMultiplier(){
    return valueMultiplier;
}

int Addition::getOscillatorCount(){
    return oscillatorCount;
}

int Addition::copyIdsToArray(int *idArray){
    for (int i = 0; i < oscillatorCount; i++){
        idArray[i] = oscillatorIds[i];
    }
    return oscillatorCount;
}
