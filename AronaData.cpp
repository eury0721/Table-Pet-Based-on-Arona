#include "AronaData.h"

AronaData::AronaData(QWidget* parent)
	: AronaMainWindow(parent)
{
	getResource();//获取资源
}

AronaData::~AronaData()
{
	saveInitTxtFIle();//保存
}

std::string AronaData::intToString(int num)
{
    std::string st = "";
    while (num)
    {
        st = (char)(num % 10 + '0') + st;
        num /= 10;
    }
    return st;
}

int AronaData::stringToInt(std::string st)
{
    int num = 0;
    for (int i = 0; i < st.size(); i++)
    {
        num *= 10;
        num = num + (int)(st[i] - '0');
    }
    return num;
}

//读取文件中的内容
QString AronaData::readFile(std::string fileUrl)
{
    std::fstream fsFile;
    //确认文件是否存在
    fsFile.open(fileUrl,std::ios::in);
    if (!fsFile.is_open())
    {
        QtMessageBox(1, fileUrl + "无法打开或不存在");
        return "";
        //exit(EXIT_FAILURE);//退出
    }

    //读入数据
    std::string fileLine = "";
    QString fileText = "";
    while (std::getline(fsFile, fileLine))
    {
        fileText += QString::fromUtf8(fileLine);
    }
    fsFile.close();

    return fileText;
}

//输出
void AronaData::outFile(std::string fileUrl, QString& outPut)
{
    std::fstream fsFile;
    //确认文件是否存在
    fsFile.open(fileUrl, std::ios::out);
    if (!fsFile.is_open())
    {
        QtMessageBox(1, fileUrl + "无法打开或不存在");
        return;
        //exit(EXIT_FAILURE);//退出
    }

    fsFile << outPut.toStdString();
    fsFile.close();
    return;
}



//报错
void AronaData::QtMessageBox(int mode, std::string st)
{
    switch (mode)
    {
    case 1:
        QMessageBox::critical(AronaMainWindow, "Arona", QString::fromUtf8(st));
        break;
    case 2:
        QMessageBox::information(AronaMainWindow, "Arona", QString::fromUtf8(st));
    }
    return;
}

////资源分析
//void AronaData::analysisLine(std::fstream& resourceFile, std::string& line)
//{
//}

//获取表情
void AronaData::getBodyExpress(std::string bodyExpressFileUrl, std::string bodyExpressName)
{
    bodyExpression[bodyExpressName].clear();
    if (!fs::exists(bodyExpressFileUrl) || !fs::is_directory(bodyExpressFileUrl))//验证目录存在
    {
        QtMessageBox(1, bodyExpressFileUrl + "表情目录不存在");
        exit(EXIT_FAILURE);
        return;
    }

    QPixmap pix;
    for (const auto& entry : fs::directory_iterator(bodyExpressFileUrl))//遍历文件夹中的文件
    {
        if (entry.is_regular_file())
        {
            if (pix.load(QString::fromUtf8(entry.path().string())))
            {
                bodyExpression[bodyExpressName].push_back(pix);
            }
            //qDebug() << entry.path().string();
        }
    }

    if (!bodyExpression[bodyExpressName].size())//防止目录中无文件
    {
        QtMessageBox(1, "无法查找到" + bodyExpressFileUrl + "表情文件");
        exit(EXIT_FAILURE);
    }
}

//获取AronaMainWindow数据
void AronaData::getAronaMainWindowData(std::string aronaMainWindowFileUrl)
{
    //获取数据
    QString aronaMainWindowText = readFile(aronaMainWindowFileUrl);
    if (aronaMainWindowText == "")
    {
        QtMessageBox(1, aronaMainWindowFileUrl + "没有数据");
        exit(EXIT_FAILURE);
    }

    aronaMainWindowFile = aronaMainWindowFileUrl;

    //正则查找其中的内容
    //originalScale
    QRegularExpression aronaMainWindowOriginalScaleReg("originalScale[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaMainWindowOriginalScaleRegMatch = aronaMainWindowOriginalScaleReg.match(aronaMainWindowText);
    if (aronaMainWindowOriginalScaleRegMatch.hasMatch())
        originalScale = aronaMainWindowOriginalScaleRegMatch.captured(1).toInt();
}

//获取aronaDeepSeek数据
void AronaData::getAronaDeepSeekData(std::string aronaDeepSeekFileUrl)
{
    //获取数据
    QString aronaDeepSeekText = readFile(aronaDeepSeekFileUrl);
    if (aronaDeepSeekText == "")
    {
        QtMessageBox(1, aronaDeepSeekFileUrl + "没有数据");
        exit(EXIT_FAILURE);
    }

    aronaDeepSeekFile = aronaDeepSeekFileUrl;

    //正则查找其中的内容
    //deepseekPhoneNumber
    QRegularExpression aronaDeepSeekDeepseekPhoneNumberReg("deepseekPhoneNumber[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekDeepseekPhoneNumberRegMatch = aronaDeepSeekDeepseekPhoneNumberReg.match(aronaDeepSeekText);
    if (aronaDeepSeekDeepseekPhoneNumberRegMatch.hasMatch())
        deepseekAccout = aronaDeepSeekDeepseekPhoneNumberRegMatch.captured(1);

    //deepseekPassWord
    QRegularExpression aronaDeepSeekDeepseekPassWordReg("deepseekPassWord[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekDeepseekPassWordRegMatch = aronaDeepSeekDeepseekPassWordReg.match(aronaDeepSeekText);
    if (aronaDeepSeekDeepseekPassWordRegMatch.hasMatch())
        deepseekPassword = aronaDeepSeekDeepseekPassWordRegMatch.captured(1);

    //deepseekUrl
    QRegularExpression aronaDeepSeekdeepseekUrlReg("deepseekUrl[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekdeepseekUrlRegMatch = aronaDeepSeekdeepseekUrlReg.match(aronaDeepSeekText);
    if (aronaDeepSeekdeepseekUrlRegMatch.hasMatch())
        deepseekUrl = aronaDeepSeekdeepseekUrlRegMatch.captured(1);

    //deepseekthink
    QRegularExpression aronaDeepSeekdeepseekthinkReg("deepseekthink[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekdeepseekthinkRegMatch = aronaDeepSeekdeepseekthinkReg.match(aronaDeepSeekText);
    if (aronaDeepSeekdeepseekthinkRegMatch.hasMatch())
        dsThink = aronaDeepSeekdeepseekthinkRegMatch.captured(1).toInt();

    //deepseeksearch
    QRegularExpression aronaDeepSeekdeepseeksearchReg("deepseeksearch[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekdeepseeksearchRegMatch = aronaDeepSeekdeepseeksearchReg.match(aronaDeepSeekText);
    if (aronaDeepSeekdeepseeksearchRegMatch.hasMatch())
        dsSearch = aronaDeepSeekdeepseeksearchRegMatch.captured(1).toInt();

    //deepseekInitWord
    QRegularExpression aronaDeepSeekdeepseekInitWordReg("deepseekInitWord[\\t\\s]*\\{[\\t\\s]*([^\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekdeepseekInitWordRegMatch = aronaDeepSeekdeepseekInitWordReg.match(aronaDeepSeekText);
    if (aronaDeepSeekdeepseekInitWordRegMatch.hasMatch())
        dsInitWord = aronaDeepSeekdeepseekInitWordRegMatch.captured(1);
}

//获取aronaGPTsoVITS数据
void AronaData::getAronaGPTsoVITSData(std::string aronaGPTsoVITSFileUrl)
{
    //获取数据
    QString aronaGPTsoVITSText = readFile(aronaGPTsoVITSFileUrl);
    if (aronaGPTsoVITSText == "")
    {
        QtMessageBox(1, aronaGPTsoVITSFileUrl + "没有数据");
        exit(EXIT_FAILURE);
    }

    aronaGPTsoVITSFile = aronaGPTsoVITSFileUrl;

    //正则查找其中的内容
    //GPTsoVITSopen
    QRegularExpression aronaDeepSeekGPTsoVITSopenReg("GPTsoVITSopen[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekGPTsoVITSopenRegMatch = aronaDeepSeekGPTsoVITSopenReg.match(aronaGPTsoVITSText);
    if (aronaDeepSeekGPTsoVITSopenRegMatch.hasMatch())
        GPTopenFlag = aronaDeepSeekGPTsoVITSopenRegMatch.captured(1).toInt();

    //GPTweight
    QRegularExpression aronaDeepSeekGPTweightReg("GPTweight[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekGPTweightRegMatch = aronaDeepSeekGPTweightReg.match(aronaGPTsoVITSText);
    if (aronaDeepSeekGPTweightRegMatch.hasMatch())
        GPTWeightFile = aronaDeepSeekGPTweightRegMatch.captured(1);

    //soVITSweight
    QRegularExpression aronaDeepSeeksoVITSweightReg("soVITSweight[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeeksoVITSweightRegMatch = aronaDeepSeeksoVITSweightReg.match(aronaGPTsoVITSText);
    if (aronaDeepSeeksoVITSweightRegMatch.hasMatch())
        soVITSWeightFile = aronaDeepSeeksoVITSweightRegMatch.captured(1);

    //ReferenceText
    QRegularExpression aronaGPTsoVITSReferenceTextReg("ReferenceText[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSReferenceTextRegMatch = aronaGPTsoVITSReferenceTextReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSReferenceTextRegMatch.hasMatch())
        GPTsoVITSReferenceText = aronaGPTsoVITSReferenceTextRegMatch.captured(1);

    //ReferenceFile
    QRegularExpression aronaGPTsoVITSReferenceFileReg("ReferenceFile[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSReferenceFileRegMatch = aronaGPTsoVITSReferenceFileReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSReferenceFileRegMatch.hasMatch())
        GPTsoVITSReferenceFile = aronaGPTsoVITSReferenceFileRegMatch.captured(1);

    //ReferenceLanguage
    QRegularExpression aronaGPTsoVITSReferenceLanguageReg("ReferenceLanguage[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSReferenceLanguageRegMatch = aronaGPTsoVITSReferenceLanguageReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSReferenceLanguageRegMatch.hasMatch())
        GPTsoVITSReferenceLanguage = aronaGPTsoVITSReferenceLanguageRegMatch.captured(1);

    //GPTsoVITSaddress
    QRegularExpression aronaGPTsoVITSaddressReg("GPTsoVITSaddress[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSaddressRegMatch = aronaGPTsoVITSaddressReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSaddressRegMatch.hasMatch())
        GPTsoVITSAddress = aronaGPTsoVITSaddressRegMatch.captured(1);

    //GPTsoVITSport
    QRegularExpression aronaGPTsoVITSportReg("GPTsoVITSport[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSportRegMatch = aronaGPTsoVITSportReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSportRegMatch.hasMatch())
        GPTsoVITSPort = aronaGPTsoVITSportRegMatch.captured(1);

}

//获取资源
void AronaData::getResource()
{
    //获取资源
    QString resourceList = readFile(".\\resource\\List.txt");
    if (resourceList == "")
    {
        QtMessageBox(1, "List.txt没有数据");
        exit(EXIT_FAILURE);
    }

    //正则查找其中的内容
    //body.nomol
    QRegularExpression bodyNomolReg("body.nomol[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch bodyNomolRegMatch = bodyNomolReg.match(resourceList);
    if (bodyNomolRegMatch.hasMatch())
        getBodyExpress(".\\resource\\" + bodyNomolRegMatch.captured(1).toStdString() + "\\", "body.nomol");

    //AronaMainWindow
    QRegularExpression aronaMainWindowReg("AronaMainWindow[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaMainWindowRegMatch = aronaMainWindowReg.match(resourceList);
    if (aronaMainWindowRegMatch.hasMatch())
        getAronaMainWindowData(".\\resource\\" + aronaMainWindowRegMatch.captured(1).toStdString());

    //AronaDeepSeek
    QRegularExpression aronaDeepSeekReg("AronaDeepSeek[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekRegMatch = aronaDeepSeekReg.match(resourceList);
    if (aronaDeepSeekRegMatch.hasMatch())
        getAronaDeepSeekData(".\\resource\\" + aronaDeepSeekRegMatch.captured(1).toStdString());

    //AronaGPTsoVITS
    QRegularExpression aronaGPTsoVITSReg("AronaGPTsoVITS[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSRegMatch = aronaGPTsoVITSReg.match(resourceList);
    if (aronaGPTsoVITSRegMatch.hasMatch())
        getAronaGPTsoVITSData(".\\resource\\" + aronaGPTsoVITSRegMatch.captured(1).toStdString());

}

//保存初始化文件
void AronaData::saveInitTxtFIle()
{
    //本质是把上述代码复制下来把读取改成替换



    //aronaMainWindowFile

    QString aronaMainWindowText = readFile(aronaMainWindowFile);
    if (aronaMainWindowText == "")
    {
        QtMessageBox(1, aronaMainWindowFile + "没有数据");
        exit(EXIT_FAILURE);
    }

    QRegularExpression aronaMainWindowOriginalScaleReg("originalScale[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaMainWindowOriginalScaleRegMatch = aronaMainWindowOriginalScaleReg.match(aronaMainWindowText);
    if (aronaMainWindowOriginalScaleRegMatch.hasMatch())
        aronaMainWindowText.replace(aronaMainWindowOriginalScaleReg, QString("originalScale\n{\n\t%1\n}\n").arg(originalScale));
    //qDebug() << aronaMainWindowText;

    outFile(aronaMainWindowFile, aronaMainWindowText);

    

    //aronaDeepSeekFile

    QString aronaDeepSeekText = readFile(aronaDeepSeekFile);
    if (aronaDeepSeekText == "")
    {
        QtMessageBox(1, aronaDeepSeekFile + "没有数据");
        exit(EXIT_FAILURE);
    }

    //正则查找其中的内容
    //deepseekPhoneNumber
    QRegularExpression aronaDeepSeekDeepseekPhoneNumberReg("deepseekPhoneNumber[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekDeepseekPhoneNumberRegMatch = aronaDeepSeekDeepseekPhoneNumberReg.match(aronaDeepSeekText);
    if (aronaDeepSeekDeepseekPhoneNumberRegMatch.hasMatch())
        aronaDeepSeekText.replace(aronaDeepSeekDeepseekPhoneNumberReg, QString("deepseekPhoneNumber\n{\n\t%1\n}\n").arg(deepseekAccout));

    //deepseekPassWord
    QRegularExpression aronaDeepSeekDeepseekPassWordReg("deepseekPassWord[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekDeepseekPassWordRegMatch = aronaDeepSeekDeepseekPassWordReg.match(aronaDeepSeekText);
    if (aronaDeepSeekDeepseekPassWordRegMatch.hasMatch())
        aronaDeepSeekText.replace(aronaDeepSeekDeepseekPassWordReg, QString("deepseekPassWord\n{\n\t%1\n}\n").arg(deepseekPassword));

    //deepseekUrl
    QRegularExpression aronaDeepSeekdeepseekUrlReg("deepseekUrl[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekdeepseekUrlRegMatch = aronaDeepSeekdeepseekUrlReg.match(aronaDeepSeekText);
    if (aronaDeepSeekdeepseekUrlRegMatch.hasMatch())
        aronaDeepSeekText.replace(aronaDeepSeekdeepseekUrlReg, QString("deepseekUrl\n{\n\t%1\n}\n").arg(deepseekUrl));

    //deepseekthink
    QRegularExpression aronaDeepSeekdeepseekthinkReg("deepseekthink[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekdeepseekthinkRegMatch = aronaDeepSeekdeepseekthinkReg.match(aronaDeepSeekText);
    if (aronaDeepSeekdeepseekthinkRegMatch.hasMatch())
        aronaDeepSeekText.replace(aronaDeepSeekdeepseekthinkReg, QString("deepseekthink\n{\n\t%1\n}\n").arg(dsThink ? "1" : "0"));

    //deepseeksearch
    QRegularExpression aronaDeepSeekdeepseeksearchReg("deepseeksearch[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekdeepseeksearchRegMatch = aronaDeepSeekdeepseeksearchReg.match(aronaDeepSeekText);
    if (aronaDeepSeekdeepseeksearchRegMatch.hasMatch())
        aronaDeepSeekText.replace(aronaDeepSeekdeepseeksearchReg, QString("deepseeksearch\n{\n\t%1\n}\n").arg(dsSearch ? "1" : "0"));

    //deepseekInitWord
    QRegularExpression aronaDeepSeekdeepseekInitWordReg("deepseekInitWord[\\t\\s]*\\{[\\t\\s]*([^\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekdeepseekInitWordRegMatch = aronaDeepSeekdeepseekInitWordReg.match(aronaDeepSeekText);
    if (aronaDeepSeekdeepseekInitWordRegMatch.hasMatch())
        aronaDeepSeekText.replace(aronaDeepSeekdeepseekInitWordReg, QString("deepseekInitWord\n{\n%1\n}\n").arg(dsInitWord));

    //qDebug() << aronaDeepSeekText;

    outFile(aronaDeepSeekFile, aronaDeepSeekText);


    //aronaGPTsoVITSText
    
    //获取数据
    QString aronaGPTsoVITSText = readFile(aronaGPTsoVITSFile);
    if (aronaGPTsoVITSText == "")
    {
        QtMessageBox(1, aronaGPTsoVITSFile + "没有数据");
        exit(EXIT_FAILURE);
    }

    //正则查找其中的内容
    //GPTsoVITSopen
    QRegularExpression aronaDeepSeekGPTsoVITSopenReg("GPTsoVITSopen[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekGPTsoVITSopenRegMatch = aronaDeepSeekGPTsoVITSopenReg.match(aronaGPTsoVITSText);
    if (aronaDeepSeekGPTsoVITSopenRegMatch.hasMatch())
        //GPTopenFlag = aronaDeepSeekGPTsoVITSopenRegMatch.captured(1).toInt();
        aronaGPTsoVITSText.replace(aronaDeepSeekGPTsoVITSopenReg, QString("GPTsoVITSopen\n{\n\t%1\n}\n").arg(GPTopenFlag ? "1" : "0"));

    //GPTweight
    QRegularExpression aronaDeepSeekGPTweightReg("GPTweight[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeekGPTweightRegMatch = aronaDeepSeekGPTweightReg.match(aronaGPTsoVITSText);
    if (aronaDeepSeekGPTweightRegMatch.hasMatch())
        aronaGPTsoVITSText.replace(aronaDeepSeekGPTweightReg, QString("GPTweight\n{\n\t%1\n}\n").arg(GPTWeightFile));

    //soVITSweight
    QRegularExpression aronaDeepSeeksoVITSweightReg("soVITSweight[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaDeepSeeksoVITSweightRegMatch = aronaDeepSeeksoVITSweightReg.match(aronaGPTsoVITSText);
    if (aronaDeepSeeksoVITSweightRegMatch.hasMatch())
        aronaGPTsoVITSText.replace(aronaDeepSeeksoVITSweightReg, QString("soVITSweight\n{\n\t%1\n}\n").arg(soVITSWeightFile));

    //ReferenceText
    QRegularExpression aronaGPTsoVITSReferenceTextReg("ReferenceText[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSReferenceTextRegMatch = aronaGPTsoVITSReferenceTextReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSReferenceTextRegMatch.hasMatch())
        aronaGPTsoVITSText.replace(aronaGPTsoVITSReferenceTextReg, QString("ReferenceText\n{\n\t%1\n}\n").arg(GPTsoVITSReferenceText));

    //ReferenceFile
    QRegularExpression aronaGPTsoVITSReferenceFileReg("ReferenceFile[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSReferenceFileRegMatch = aronaGPTsoVITSReferenceFileReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSReferenceFileRegMatch.hasMatch())
        aronaGPTsoVITSText.replace(aronaGPTsoVITSReferenceFileReg, QString("ReferenceFile\n{\n\t%1\n}\n").arg(GPTsoVITSReferenceFile));

    //ReferenceLanguage
    QRegularExpression aronaGPTsoVITSReferenceLanguageReg("ReferenceLanguage[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSReferenceLanguageRegMatch = aronaGPTsoVITSReferenceLanguageReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSReferenceLanguageRegMatch.hasMatch())
        aronaGPTsoVITSText.replace(aronaGPTsoVITSReferenceLanguageReg, QString("ReferenceLanguage\n{\n\t%1\n}\n").arg(GPTsoVITSReferenceLanguage));

    //GPTsoVITSaddress
    QRegularExpression aronaGPTsoVITSaddressReg("GPTsoVITSaddress[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSaddressRegMatch = aronaGPTsoVITSaddressReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSaddressRegMatch.hasMatch())
        //GPTsoVITSAddress = aronaGPTsoVITSaddressRegMatch.captured(1);
        aronaGPTsoVITSText.replace(aronaGPTsoVITSaddressReg, QString("GPTsoVITSaddress\n{\n\t%1\n}\n").arg(GPTsoVITSAddress));

    //GPTsoVITSport
    QRegularExpression aronaGPTsoVITSportReg("GPTsoVITSport[\\t\\s]*\\{[\\t\\s]*([^}\\t\\s]*)[\\t\\s]*\\}");
    QRegularExpressionMatch aronaGPTsoVITSportRegMatch = aronaGPTsoVITSportReg.match(aronaGPTsoVITSText);
    if (aronaGPTsoVITSportRegMatch.hasMatch())
        aronaGPTsoVITSText.replace(aronaGPTsoVITSportReg, QString("GPTsoVITSport\n{\n\t%1\n}\n").arg(GPTsoVITSPort));
    //qDebug() << aronaGPTsoVITSText;

    outFile(aronaGPTsoVITSFile, aronaGPTsoVITSText);
}


