#include "irtester.hpp"

#include "../loader/irloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"

IRTester::IRTester() {
    setImageLoader(new IrLoader);

    addPreProcessor(new GaussianBlur);
    addPreProcessor(new LBP);
}

std::string IRTester::name()
{
    return "IR Tester";
}
