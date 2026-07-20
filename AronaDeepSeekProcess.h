#ifndef _AronaDeepSeekProcess_H_
#define _AronaDeepSeekProcess_H_

#include <QtWidgets/QMainWindow>
//#include <qdebug.h>
#include <qdialog.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <qprocess.h>
#include <qdatetime.h>
#include <qplaintextedit.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qregularexpression.h>
#include <regex>
#include <vector>
#include <iostream>
#include <string>

class Arona;

//class LastReply
//{
//public:
//    QString replyCN;
//    QString replyJP;
//};

//DS通信状态机
enum DSProtocolState
{
    StateIdle,//空闲
    StateSendingAccount,//等待发送账号
    StateWaitingNumberOver,//等待回复phoneNumberOver
    StateSendingPassword,//等待发送密码
    StateWaitingPassWordOver,//等待回复passWordOver
    StateWaitingLogin,//等待登录结果(loginSuc/loginError)
    StateSendingThinkFlag,//等待发送深度思考标志
    StateWaitingThinkFlagOver,//等待回复thinkFlagOver
    StateSendingSearchFlag,//等待发送智能搜索标志
    StateWaitingSearchFlagOver,//等待回复searchFlagOver
    StateSendingInit,//等待发送/Init 或/nInit
    StateWaitingInit,//等待initOver或initChatOver
    StateSendingInitText,// 等待输入初始化文本（对应/Init 分支）
    StateWaitingChatUrl,// 等待聊天窗口网址（对应/Init 分支输出）
    StateSendingUrl,// 等待输入网址（对应/nInit分支）
    StateWaitingInitChat,//等待输出initChatOver或pointlessUrl（对应/nInit分支）
    StateSendingReCommand,//等待重发命令词（对应/nInit分支）
    StateWaitingReCommandReply,//等待对命令词的回复（对应/nInit分支）
    StateSendingChatOutput,//正常聊天输出
    StateWaitingChatInput,//正常聊天输入
    StateExiting,//退出中
    StateWaitingGPTsoVITSInit//等待GPTsoVITS启动
};
/*脚本通信协议，对于脚本端
输入输出时序规范流程:in为外部向脚本输入 out为脚本向外部输出
in 账户
out phoneNumberOver\n
in 密码
out passWordOver\n
out loginSuc\n或loginError\n 等待30秒若无法登录则关闭程序
in 深度思考标志
out thinkFlagOver\n
in 智能搜索标志
out searchFlagOver\n
in /Init或/nInit
out initOver\n
    若上为/Init则
        in 初始化文本
        out 聊天窗口的网址
    若上为/nInit则
        in 聊天窗口的网址 若网页无效则out pointlessUrl
        out initChatOver\n
        in 重发命令词
        out 回复
初始化结束由此进入循环
    in 输入文本
    out 输出文本
    ……
    in /exit
    out exitOver\n
*/

class AronaDeepSeekProcess : public QObject
{
	Q_OBJECT
public:
	AronaDeepSeekProcess(QWidget* parent = nullptr);
	~AronaDeepSeekProcess();

    //输入：以子进程向本类输出文本
    //输出：本类向子进程输出文本为
    
    //父指针
    QWidget* AronaMainWindow = nullptr;
    Arona* arona = nullptr;

    //初始化进程和状态机
    void initDsProccess();
    //启动子进程
    void startDSProcess();
    //发送数据
    void sendDSData(QString& dataLine);
    QString DSAccount = "";//账户
    QString DSPassword = "";//密码
    QString DSThink = "0";//深度思考标志
    QString DSSearch = "0";//智能搜索标志
    bool DSinitFlag = 0;//聊天初始化标志位，置0发送/Init，置1发送/nInit
    QString DSUrl = "";//聊天网址
    QString DSInitWord = "";//初始化文本
    //以上数据请在状态机初始时进行初始化


    QString DSlineIn = "";//输入文本缓冲区
    QString DSlineOut = "";//输出文本缓冲区
    bool DSChatFlag = 0;//输出限制标志位，置1则为允许输入框输出
    bool DSinputFlag = 0;//输处标志位，置1则有输出
    bool DSoutputFlag = 0;//输入标志位，置1则为有输入
    QString DSinit = "/Init";//初始化指令
    QString DSninit = "/nInit";//初始化指令

    //对话输出格式化打包
    void tellForm(QString actionModel);
    //对话窗口
    QDialog* tellDialog;//对话输入窗口
    QLineEdit* tellLine;//对话输入对话框
    QPushButton* lineOut;//发送对话
    //发送对话按键按下
    void on_lineOut_clicked();
    //初始化对话输出窗口
    void inittellDialog();
    //显示对话输出窗口
    void showtellDialog();
    QDialog* replyDialog;//对话回复窗口
    QTextEdit* replyLine;//对话回复框
    const int replyLineMaxWidth = 300;//对话回复框最大宽度
    QTimer* scanTimer;//扫描计时器
    QTimer* replyDialogHideTimer;//回复窗口关闭计时器
    //初始化回复窗口
    void initReplyDialog();
    //显示回复框
    void showReplyDialog();
    //回复窗口自适应
    void adjustReplyDialogSize(); 
    QString replyCN = "";//中文输出
    QString replyJP = "";//日文输出
    QRegularExpression replyAction;//回复/Action内容
    QRegularExpressionMatch replyActionMatch;
    QRegularExpression replyScanTime;//回复/ScanTime内容
    QRegularExpressionMatch replyScanTimeMatch;
    QRegularExpression replyReply;//回复/Reply内容
    QRegularExpressionMatch replyReplyMatch;
    QRegularExpression replyReplyJP;//回复/ReplyJP内容
    QRegularExpressionMatch replyReplyJPMatch;
    //分析输出
    void analyseReply(QString unprocessedText);

    bool aronaDeepSeekProcessKillFlag = 0;//进程关闭标志

    //状态转移和输入分析
    void analyseOutput();

public slots:
    //读取子进程输入
    void onReadyRead();
    //限制对话窗口输出
    void tellOutputLimit(bool& newchatFlag);

signals:
    //Url改变信号槽，发送向settingsDialog->dsUrl
    //void DSUrlChanged(QString& newUrl);
    //DSChatFlag改变信号,设定是否允许输入
    void DSChatFlagChanged(bool& newFlag);
    //chatIput输入信号
    void DSChatInputFlag();

private:
    //指向deepseek脚本的指针
    QProcess* DSprocess = nullptr;
    //通信状态
    DSProtocolState DSstate = StateIdle;
};


#endif // !_AronaDeepSeekProcess_H_

/*
本类未与外部相连
请完成
将信号连接
*/