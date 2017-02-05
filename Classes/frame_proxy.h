#pragma once
#include <functional>
void InstallFrameMessageHanlder(std::function<int(unsigned int, unsigned int)> fn);
void UnInstallFrameMessageHanlder();
