#include "pch.h"
#include "shell.h"
#include <LoggerAPI.h>
#include <ScheduleAPI.h>

std::string& trim(std::string& s)
{
	if (s.empty())
	{
		return s;
	}
	//s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

std::string getLevelName() {
	std::ifstream properties("server.properties");
	string line;
	while (std::getline(properties, line)) {// ��server.properties�ļ��е�ÿһ���ַ����뵽string line��
		if (line.find("level-name") != string::npos) {// ����level-name��
			string levelName = line.substr(line.find("=") + 1);
			return trim(levelName);
		}
	}
	return "";
}

Shell shell;
bool errorMode = false;
int startUnmined() {
	string levelName = getLevelName();
	if (levelName == "") {
		Logger logger("BDSLM");
		logger.warn << L"�޷���server.properties�ж�ȡlevel-name�ʹ��Ĭ��ֵ level��" << logger.endl;
		levelName = "level";
	}
	bool status = true;
	status = shell.RunProcess(".\\plugins\\BDSLM\\unmined\\unmined-cli.exe web render --world=\"./worlds/" + levelName + "\" --output=\"./plugins/BDSLM/unmined-web/\" --imageformat=webp -c --zoomin=-2 --zoomout=4");
	Schedule::repeat([]() {
		string line;
		shell.GetOutput(1, line);
		/*if (line.find("Elapsed time total") != string::npos) {
			Logger logger;
			logger.info << "��ͼ������ϣ�" << logger.endl;
		}
		else if (line.find("exception") != string::npos) {
			Logger logger;
			logger.error << "��ͼ����ʧ�ܣ�" << logger.endl;
			errorMode = true;
		}
		else {

		}
		if (errorMode) {
			std::cout << line;
		}*/
	}, 40);
	return status;
}