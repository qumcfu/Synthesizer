#ifndef ADDITION_H
#define ADDITION_H


class Addition{
public:
    Addition();
    void setOscillatorIds(int* ids, int oscillatorCount);
    void setValueMultiplier(float vm);
    float getValueMultiplier();
    int getOscillatorCount();
    int copyIdsToArray(int* idArray);
private:
    int* oscillatorIds;
    int oscillatorCount;
    float valueMultiplier;
};

#endif // ADDITION_H
