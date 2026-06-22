//IPanTiltController.hpp

#pragma once

namespace slam_robot {

struct PanTiltAngles {
    double Pan; 
    double Tilt;
}; 



class IPanTiltController {

public:

virtual void setAngles(PanTiltAngles angles) = 0;

virtual PanTiltAngles getCommandedAngles() const = 0;


virtual ~IPanTiltController() = default;

};

}
