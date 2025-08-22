//
// @Time     : 2023/11/12 11:38
// @Author   : Yang
// @Software : CLion
// 数据格式转换

#ifndef USVCONTROLLER_DataFormatTools_HPP
#define USVCONTROLLER_DataFormatTools_HPP

#include "string"
#include "iconv.h"

namespace YFormatTools
{
    inline std::string bytesToHexStr(const unsigned char data_char[], int data_size)
    {
        std::string testStr;
        for(int i=0; i<data_size; i++)
        {
            char hexChar[4];
            unsigned char  one_char = data_char[i];
            snprintf((char *)hexChar, 4 ,"%02x ", one_char);
            testStr += hexChar;
        }
        return testStr ;
    }

    /** 字符解码 **/
    inline std::string bytesToStr(char data_char, int data_size)
    {
//        std::string from_encoding = "UTF-8";
        std::string from_encoding = "ASCII";
        iconv_t cd = iconv_open(&data_char, from_encoding.c_str());
        if (cd == (iconv_t)-1)
        {
            std::cerr << "iconv打开 失败\n";
            return "";
        }
        char *in_buf = &data_char;
        size_t in_len = data_size;
        size_t out_len = data_char*2;
        char *out_buf = new char[out_len];
        size_t res = iconv(cd, &in_buf, reinterpret_cast<size_t *>(&data_size), &out_buf, &out_len);
        if (res == (size_t)-1)
        {
            std::cerr << "iconv failed\n";
            return "";
        }
        std::string output(out_buf, out_len-out_len);
        std::cout << "Output: " << output << std::endl;
        delete[] out_buf;
        iconv_close(cd);
        return 0;
        return "" ;
    }

    /** 分割字符串为数组 **/
    inline void splitString(const std::string& one_str, const std::string& element_str, std::vector<std::string>& result_ary)
    {
        std::string::size_type pos1, pos2;
        pos2 = one_str.find(element_str);
        pos1 = 0;
        while(std::string::npos != pos2)
        {
            result_ary.push_back(one_str.substr(pos1, pos2-pos1));

            pos1 = pos2 + element_str.size();
            pos2 = one_str.find(element_str, pos1);
        }
        if(pos1 != one_str.length())
            result_ary.push_back(one_str.substr(pos1));
    }

    /** int转为大端4字节 **/
    inline void intToBytes(const int one_num, unsigned char char_ary[])
    {
//        char_ary[0] = (one_num >> 24) & 0xFF; // 获取最高8位并存储到第1个字节
//        char_ary[1] = (one_num >> 16) & 0xFF; // 获取次高8位并存储到第2个字节
//        char_ary[2] = (one_num >> 8) & 0xFF; // 获取次低8位并存储到第3个字节
//        char_ary[3] = one_num & 0xFF; // 获取最低8位并存储到第4个字节

        char_ary[0] = (one_num >> 24); // 获取最高8位并存储到第1个字节
        char_ary[1] = (one_num >> 16); // 获取次高8位并存储到第2个字节
        char_ary[2] = (one_num >> 8) ; // 获取次低8位并存储到第3个字节
        char_ary[3] = one_num; // 获取最低8位并存储到第4个字节
    }

//    inline char[] stringToChar(std::string get_str)
//    {
//        // string 转成 char
//        char get_char[get_str.length()];
//        strcpy(get_char,get_str.c_str());
//        return get_char ;
//    }

    /** int转为大端4字节 **/
    inline void ChangeBitToTure(unsigned char * one_num, int index)
    {
        // 赋1
        //send_char_1 |= (1 << 0);
        //send_char_1 |= (1 << 1);
        //send_char_1 |= (1 << 2);
        // 赋0
        //send_char_1 &= ~(1 << 1);
    }


}

// 主机转网络 htons() htonl()
// 网络转主机 ntohs() ntohl()

#endif //USVCONTROLLER_DataFormatTools_HPP