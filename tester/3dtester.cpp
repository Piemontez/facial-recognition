#include "3dtester.hpp"

#include "../loader/3dloader.hpp"
#include "../processor/roi.hpp"
#include "../processor/posecorretion.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

ThreeDTester::ThreeDTester(bool poseCorrection) {
    auto imgLoader = new ThreeLoader;
    setImageLoader(imgLoader);

    addPreProcessorOrderFixed(new ROI());
    if (poseCorrection)
        addPreProcessorOrderFixed(new PoseCorrection(imgLoader));

    addPreProcessor(new LBP);
    addPreProcessor(new GaussianBlur);
    addPreProcessor(new DCT);
}

std::string ThreeDTester::name()
{
    return "3D Tester";
}
