// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "ashe/singleton_process.hpp"
#include <windows.h>

int main() {
    ashe::SingletonProcess::Instance()->markAndCheckStartup("SystemThemePlugin_JDUIW9JWUD13S#");
    if (!ashe::SingletonProcess::Instance()->isPrimary()) {
        ashe::SingletonProcess::Release();
        return 0;
    }

    ashe::SingletonProcess::Instance()->registerSecondaryCallback([](std::string secondaryCml) {
        std::cout << "New Process:" << secondaryCml << std::endl;
    });

    Sleep(100000);
    ashe::SingletonProcess::Release();
}
