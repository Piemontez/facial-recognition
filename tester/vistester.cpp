#include "vistester.hpp"

#include "../loader/visloader.hpp"
#include "../processor/gaussianblur.hpp"
#include "../processor/lbp.hpp"

VisTester::VisTester() {
    setImageLoader(new VisLoader);

    addPreProcessor(new GaussianBlur);
    addPreProcessor(new LBP);
}

std::string VisTester::name()
{
    return "VIS Tester";
}
