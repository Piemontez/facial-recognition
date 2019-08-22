#ifndef THREEDTESTER_HPP
#define THREEDTESTER_HPP

#include "../tester.hpp"
#ifdef HASCONTRIB
#include "opencv4/opencv2/contrib/contrib.hpp"
#endif

class ThreeDTester : public Tester
{
#ifdef HASCONTRIB
    Ptr<FaceRecognizer> model;
#endif
public:
    ThreeDTester();

    std::string name() override;
};

#endif // THREEDTESTER_HPP
