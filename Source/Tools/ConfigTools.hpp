//
// @Time     : 2024/5/28 13:36
// @Author   : Yang
// @Software : CLion
//

#ifndef AUV5PI_CONFIGTOOLS_H
#define AUV5PI_CONFIGTOOLS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

namespace YConfigTools
{
    inline std::map<std::string, std::map<std::string, std::string>> parseIniFile(const std::string& filename)
    {
        std::ifstream file(filename);
        std::map<std::string, std::map<std::string, std::string>> iniData;
        std::string currentSection;

        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                // 去除行首尾空格
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);

                // 如果是注释或空行，则跳过
                if (line.empty() || line[0] == ';')
                {
                    continue;
                }
                else if (line[0] == '[' && line.back() == ']')
                {   // 如果是节（section）行，则更新当前节
                    currentSection = line.substr(1, line.size() - 2);
                }
                else
                {// 否则是键值对行
                    std::istringstream lineStream(line);
                    std::string key, value;
                    if (std::getline(lineStream, key, '=') && std::getline(lineStream, value))
                    {
                        // 去除键和值的空格
                        key.erase(0, key.find_first_not_of(" \t\r\n"));
                        key.erase(key.find_last_not_of(" \t\r\n") + 1);
                        value.erase(0, value.find_first_not_of(" \t\r\n"));
                        value.erase(value.find_last_not_of(" \t\r\n") + 1);
                        // 将键值对存入map
                        iniData[currentSection][key] = value;
                    }
                }
            }
            file.close();
        }
        else
        {
            std::cerr << "Unable to open file: " << filename << std::endl;
        }
        return iniData;
    }
}
#endif //AUV5PI_CONFIGTOOLS_H
