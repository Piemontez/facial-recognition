#ifndef IRTESTER_HPP
#define IRTESTER_HPP

#include "../tester.hpp"

class IRTester : public Tester
{
public:
    IRTester();

    std::string name() override;
};

#endif // IRTESTER_HPP
