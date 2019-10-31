#include "3dtester.hpp"

#include "../loader/3dloader.hpp"
#include "../processor/posecorretion.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

ThreeDTester::ThreeDTester() {
    auto imgLoader = new ThreeLoader;
    setImageLoader(imgLoader);

    addPreProcessorOrderFixed(new PoseCorrection(imgLoader));

    addPreProcessor(new GaussianBlur);
    addPreProcessor(new LBP);
    addPreProcessor(new DCT);
}

std::string ThreeDTester::name()
{
    return "3D Tester";
}
