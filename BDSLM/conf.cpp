#include "pch.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#include <yaml-cpp/yaml.h>

unsigned short int port;
unsigned short int apiPort;

void parseConfFile() {
    YAML::Node conf;
    try {
        conf = YAML::LoadFile("./plugins/BDSLM/config.yaml");
    }
    catch (YAML::BadFile& e) {
        Logger logger("BDSLM");
        logger.error << L"�����ļ���ȡʧ�ܣ������¼���ȫ�°�װBDSLM��" << logger.endl;
        // To-Do: �Զ����������ļ�
        port = 5000;
        apiPort = 5001;
    }
    port = conf["webServer"]["port"].as<unsigned short int>();
    apiPort = conf["apiServer"]["port"].as<unsigned short int>();
}