#include "irloader.hpp"

#include "iostream"
#include "json/reader.h"
#include "opencv4/opencv2/opencv.hpp"

const std::string rap3dfFolder = "/home/piemontez/Projects/piemontez/rap3df-database/";
const std::string rap3df02file = "rap3df_data_02/database.json";

IrLoader::IrLoader() {
    //load json file
    std::FILE* jsonFile = std::fopen((rap3dfFolder+rap3df02file).c_str(),"r");
    if (jsonFile) {

        Json::Reader reader;
        std::string json;
        char buf[2];
        while (std::fgets(buf, sizeof buf, jsonFile) != nullptr) {
            json.push_back(buf[0]);
        }
        std::fclose(jsonFile);

        reader.parse(json, root);
    } else {
        std::cout << "Arquivo " << (rap3dfFolder+rap3df02file) << " não encontrado" << std::endl;
    }
    if (!root["_faces"].empty()) {
        Json::Value facesUUIDs = root["_faces"]; //Lista com população

        for (auto && i = facesUUIDs.begin(); i != facesUUIDs.end(); i++)
        {
            std::string uuid = (*i).asString();

            Json::Value voluntary = root[uuid];
            if (voluntary.empty() && !voluntary["front"].begin()->isObject()) continue;

            //coleta primeira imagem frontal
            Json::Value front = *voluntary["front"].begin();
            //endereço imagem frontal
            std::string dethPath = front["ir_with_bg"].asString();

            _files.push_back(rap3dfFolder + dethPath);
            _labels.push_back(uuid);
        }
    }
}

std::vector<cv::Mat> IrLoader::images()
{
    std::vector<cv::Mat> images;

    for (auto && filePath: _files)
    {
        images.push_back( cv::imread(filePath) );
    }

    return images;
}

std::vector<int> IrLoader::labels()
{
    std::vector<int> labels;

    int pos = 0;
    for (auto && label: _labels)
    {
        labels.push_back( pos++ );
    }

    return labels;
}

std::vector<int> IrLoader::flags()
{
    std::vector<int> flags;

    for (auto && label: _labels)
    {
        flags.push_back(FRONTAL | IR);
    }

    return flags;
}
