#include "3dtester.hpp"

#include "../loader/3dloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"

ThreeDTester::ThreeDTester() {
    setImageLoader(new ThreeLoader);

    addPreProcessor(new GaussianBlur);
    addPreProcessor(new LBP);
}

std::string ThreeDTester::name()
{
    return "3D Tester";
}
