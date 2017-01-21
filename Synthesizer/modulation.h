#ifndef MODULATION_H
#define MODULATION_H

class Modulation{
public:
    Modulation();
    void setCarrierId(int id);
    void setModulatorId(int id);
    void setModulationIndex(float index);
    int getCarrierId();
    int getModulatorId();
    float getModulationIndex();
private:
    int carrierId;
    int modulatorId;
    float modulationIndex;
};

#endif // MODULATION_H
