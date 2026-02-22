#pragma once

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
