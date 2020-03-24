#ifndef THREEDTESTER_HPP
#define THREEDTESTER_HPP

#include "../tester.hpp"

class ThreeDTester : public Tester
{
public:
    ThreeDTester(bool poseCorrection);

    std::string name() override;
};

#endif // THREEDTESTER_HPP
