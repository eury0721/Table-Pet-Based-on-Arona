#ifndef _AronaData_H_
#define _AronaData_H_

#include <qpainter.h>
#include <qmessagebox.h>
#include <qwidget.h>
#include <qplaintextedit.h>
#include <qregularexpression.h>
#include <qfile.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

class AronaData //保存程序中的数据
{
public:

    AronaData(QWidget* parent = nullptr);
    ~AronaData();
    QWidget* AronaMainWindow = nullptr;

    //显示资源
    std::string expressName[1] = { "body.nomol" };//表情形式
    std::map<std::string, std::vector<QPixmap>> bodyExpression;//存储表情
    //std::vector<QPixmap> bodyExpression;//缓存当前
    int originalScale = 100;//原始缩放比
    QSize originalSize;//原始窗口大小

    //账户数据
    QString deepseekAccout = "";//DS账户
    QString deepseekPassword = "";//DS密码
    QString deepseekUrl = "";//当前所在的Url
    QString dsInitWord = "";//DS初始化提示词
    bool dsThink = 0;//深度思考标志
    bool dsSearch = 0;//智能查找标志

    //GPT-soVITS数据
    bool GPTopenFlag = 0;//是否启用GPT-soVITS
    QString GPTWeightFile = "";//GPT模型
    QString soVITSWeightFile = "";//soVITS模型
    QString GPTsoVITSReferenceFile = "";//音频参考文件
    QString GPTsoVITSReferenceText = "";//音频文件中的文本
    QString GPTsoVITSReferenceLanguage = "";//音频文件中的文本的语言
    QString GPTsoVITSAddress = "";//GPTsoVITS通信地址
    QString GPTsoVITSPort = "";//GPTsoVITS通信端口
    QString GPTsoVITSTTSTextLanguage = "ja";//TTS文本的语言

private:

    std::string aronaMainWindowFile = "";//aronaMainWindow文件地址
    std::string aronaDeepSeekFile = "";//aronaDeepSeek文件地址
    std::string aronaGPTsoVITSFile = "";//aronaGPTsoVITS文件地址

    QString readFile(std::string fileUrl);//读取文件中的内容
    void outFile(std::string fileUrl, QString& outPut);//输出
    void QtMessageBox(int mode, std::string st);//报错
    int stringToInt(std::string st);
    std::string intToString(int num);

    //void analysisLine(std::fstream& resourceFile, std::string& line);//资源分析
    void getBodyExpress(std::string bodyExpressFileUrl, std::string bodyExpressName);//获取表情
    void getAronaMainWindowData(std::string aronaMainWindowFileUrl);//获取AronaMainWindow数据
    void getAronaDeepSeekData(std::string aronaDeepSeekFileUrl);//获取aronaDeepSeek数据
    void getAronaGPTsoVITSData(std::string aronaGPTsoVITSFileUrl);//获取aronaGPTsoVITS数据
    void getResource();//获取资源
    void saveInitTxtFIle();//保存初始化文件
};

#endif // !_AronaData_H_