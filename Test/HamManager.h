#pragma once
#include <string>

struct HamConfig {
	static const int MAX_PORT_NUM;
	static const int MIN_PORT_NUM;
	HamConfig(const std::string port = "", const std::string freq = "", const std::string callSign = "");
	std::string callSign;
	std::string freq;
	std::string port;

private:
	void genDefaultPort();
};

/*
class HamManager {
public:
	HamManager();

	bool Start(const HamConfig& config);

	bool Start(const)

	~HamManager();
protected:

};
*/