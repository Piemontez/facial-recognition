#include "3dloader.hpp"

#include "iostream"
#include "json/reader.h"
#include "opencv4/opencv2/opencv.hpp"

const std::string rap3dfFolder = "/home/piemontez/Projects/piemontez/rap3df-database/";
const std::string rap3df02file = "rap3df_data_02/database.json";

ThreeLoader::ThreeLoader() {
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
            if (voluntary.empty()) continue;

            if (voluntary["front"].isArray() && voluntary["front"].begin()->isObject()) {
                //coleta primeira imagem frontal
                Json::Value front = *voluntary["front"].begin();
                //endereço imagem frontal
                std::string dethPath = front["depth_with_bg"].asString();

                _files.push_back(rap3dfFolder + dethPath);
                _labels.push_back(uuid);
                _flags.push_back(DEPTH | FRONTAL | RECOG_TRAIN | COMPARE_MAIN_TRAIN | COMPARE_TEST);
            }

            if (voluntary["right"].isArray() && voluntary["right"].begin()->isObject()) {
                //coleta primeira imagem frontal
                Json::Value front = *voluntary["front"].begin();
                //endereço imagem frontal
                std::string dethPath = front["depth_with_bg"].asString();

                _files.push_back(rap3dfFolder + dethPath);
                _labels.push_back(uuid);
                _flags.push_back(DEPTH | RITH | RECOG_TEST | COMPARE_TRAIN | COMPARE_TEST);
            }

            if (voluntary["left"].isArray() && voluntary["left"].begin()->isObject()) {
                //coleta primeira imagem frontal
                Json::Value front = *voluntary["front"].begin();
                //endereço imagem frontal
                std::string dethPath = front["depth_with_bg"].asString();

                _files.push_back(rap3dfFolder + dethPath);
                _labels.push_back(uuid);
                _flags.push_back(DEPTH | LEFT | RECOG_TEST | COMPARE_TRAIN | COMPARE_TEST);
            }

            if (voluntary["up"].isArray() && voluntary["up"].begin()->isObject()) {
                //coleta primeira imagem frontal
                Json::Value front = *voluntary["front"].begin();
                //endereço imagem frontal
                std::string dethPath = front["depth_with_bg"].asString();

                _files.push_back(rap3dfFolder + dethPath);
                _labels.push_back(uuid);
                _flags.push_back(DEPTH | TOP | RECOG_TEST | COMPARE_TRAIN | COMPARE_TEST);
            }

            if (voluntary["down"].isArray() && voluntary["down"].begin()->isObject()) {
                //coleta primeira imagem frontal
                Json::Value front = *voluntary["front"].begin();
                //endereço imagem frontal
                std::string dethPath = front["depth_with_bg"].asString();

                _files.push_back(rap3dfFolder + dethPath);
                _labels.push_back(uuid);
                _flags.push_back(DEPTH | DOWN | RECOG_TEST | COMPARE_TRAIN | COMPARE_TEST);
            }

            if (voluntary["burned"].isArray() && voluntary["burned"].begin()->isObject()) {
                //coleta primeira imagem frontal
                Json::Value front = *voluntary["front"].begin();
                //endereço imagem frontal
                std::string dethPath = front["depth_with_bg"].asString();

                _files.push_back(rap3dfFolder + dethPath);
                _labels.push_back(uuid);
                _flags.push_back(DEPTH | RANDOM | RECOG_TEST | COMPARE_TRAIN | COMPARE_TEST);
            }
        }
    }
}

std::vector<cv::Mat> ThreeLoader::images()
{
    std::vector<cv::Mat> images;

    for (auto && filePath: _files)
    {
        images.push_back( cv::imread(filePath) );
    }

    return images;
}

std::vector<int> ThreeLoader::labels()
{
    std::vector<int> labels;

    int pos = 0;
    for (auto && label: _labels)
    {
        labels.push_back( pos++ );
    }

    return labels;
}

std::vector<int> ThreeLoader::flags()
{
    std::vector<int> flags;

    for (auto && label: _flags)
    {
        flags.push_back(label);
    }

    return flags;
}
