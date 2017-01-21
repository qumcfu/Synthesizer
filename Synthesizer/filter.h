#ifndef FILTER_H
#define FILTER_H


class Filter{
public:
    enum Type {None, ChebyshevLowpass};
    Filter();
    void recalculate();
    void setCutoff(float cutoff);
    void setSampleRate(float sampleRate);
    void setRipple_dB(float ripple_dB);
    void setFilterType(Type type);
    float getCutoff();
    Type getFilterType();
    float process(float input);
    bool isActive();

private:
    Type filterType;
    float frequency;
    float sampleRate;
    float ripple_dB;
    bool active;
    double k, sg, cg;
    double coeff[4];
    double a0, a1, a2, a3, a4, a5;
    double b0, b1, b2, b3, b4, b5;
    double state0, state1, state2, state3;

};

#endif // FILTER_H
