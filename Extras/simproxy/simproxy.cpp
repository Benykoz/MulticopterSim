/*
   UDP proxy for testing MulticopterSim socket comms

   Copyright(C) 2019 Simon D.Levy

   MIT License
 */

#include <stdio.h>

#include "sockets/UdpServerSocket.hpp"
#include "sockets/UdpClientSocket.hpp"

#include "MultirotorDynamics.hpp"

static const char * HOST       = "127.0.0.1";
static const short  MOTOR_PORT = 5000;
static const short  TELEM_PORT = 5001;
static const double DELTA_T    = 0.001;

static MultirotorDynamics::params_t bigQuadParams = {

    // Dynamics: Amir's calculations
    5.30216718361085E-05,   // b
    2.23656692806239E-06,   // d
    16.47,                  // m
    0.6,                    // l
    2,                      // Ix
    2,                      // Iy
    3,                      // Iz
    3.08013E-04,            // Jr

    // maxrpm, estimated
    15000
};

class QuadXAP : public MultirotorDynamics {

    protected:

        // MultirotorDynamics method overrides

        // roll right
        virtual double u2(double * o) override
        {
            return (o[1] + o[2]) - (o[0] + o[3]);
        }

        // pitch forward
        virtual double u3(double * o) override
        {
            return (o[1] + o[3]) - (o[0] + o[2]);
        }

        // yaw cw
        virtual double u4(double * o) override
        {
            return (o[0] + o[1]) - (o[2] + o[3]);
        }

    public:

        QuadXAP(const params_t & params)
            : MultirotorDynamics(params, 4)
        {
        }

}; // class QuadXAP

int main(int argc, char ** argv)
{
    UdpServerSocket motorServer = UdpServerSocket(MOTOR_PORT);
    UdpClientSocket telemClient = UdpClientSocket(HOST, TELEM_PORT);

    QuadXAP quad = QuadXAP(bigQuadParams);

    int count = 0;

    double time = 0;

    MultirotorDynamics::pose_t pose = {0};

    quad.init(pose);

    while (true) {

        double motorvals[4] = {0};

        motorServer.receiveData(motorvals, 32);

        quad.setMotors(motorvals);

        quad.update(DELTA_T);

        MultirotorDynamics::state_t state = {0};

        quad.getState(state);
        
        // Time Gyro, Quat, Location, Rotation
        double telemetry[1];

        telemetry[0] = time;

        telemClient.sendData(telemetry, sizeof(telemetry));

        printf("%06d: %f %f %f %f\n", 
                count++, motorvals[0], motorvals[1], motorvals[2], motorvals[3]);

        time += DELTA_T;
    }

    motorServer.closeConnection();
    telemClient.closeConnection();

    return 0;
}
