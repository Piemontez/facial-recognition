#include "vistester.hpp"

#include "../loader/visloader.hpp"

VisTester::VisTester() {
    setImageLoader(new VisLoader);
}

std::string VisTester::name()
{
    return "VIS Tester";
}
