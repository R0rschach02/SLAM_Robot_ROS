#pragma once

//hier kommen nur virtuelle Funktionen rein, damit die Motorcontroller Klasse von anderen Klassen geerbt werden kann, um die Motoren zu steuern

struct JointAngles {
    double pan;
    double tilt;
};


class IMotorController {
public:
    virtual void setAngles(double pan, double tilt) = 0;
    virtual JointAngles getCurrentAngles() = 0;
    virtual ~IMotorController() = default;
};
