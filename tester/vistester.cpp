#include "vistester.hpp"

#include "../loader/visloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"
#include "../processor/dct.hpp"

VisTester::VisTester() {
    setImageLoader(new VisLoader);

    addPreProcessor(new GaussianBlur);
    addPreProcessor(new LBP);
    addPreProcessor(new DCT);
}

std::string VisTester::name()
{
    return "VIS Tester";
}
