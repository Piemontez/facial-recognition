#ifndef VISTESTER_HPP
#define VISTESTER_HPP

#include "../tester.hpp"

class VisTester : public Tester
{
public:
    VisTester();

    virtual std::string name() override;
};

#endif // VISTESTER_HPP
