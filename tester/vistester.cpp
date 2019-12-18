#include "vistester.hpp"

#include "../processor/roi.hpp"
#include "../loader/visloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

#include "opencv4/opencv2/opencv.hpp"

VisTester::VisTester() {
    setImageLoader(new VisLoader);

    addPreProcessorOrderFixed(new ROI());
//    addPreProcessor(new GaussianBlur);
//    addPreProcessor(new LBP);
    addPreProcessor(new DCT);
}

std::string VisTester::name()
{
    return "VIS Tester";
}
