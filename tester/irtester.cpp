#include "irtester.hpp"

#include "../loader/irloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

IRTester::IRTester() {
    setImageLoader(new IrLoader);

    addPreProcessor(new GaussianBlur);
    addPreProcessor(new LBP);
    //addPreProcessor(new DCT);
}

std::string IRTester::name()
{
    return "IR Tester";
}
