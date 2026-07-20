#include "Arona.h"
#include "AronaDeepSeekProcess.h"

AronaDeepSeekProcess::AronaDeepSeekProcess(QWidget* parent)
    :AronaMainWindow(parent)
    , scanTimer(new QTimer)
    , replyDialogHideTimer(new QTimer)
{
    arona = qobject_cast<Arona*>(AronaMainWindow);
    initDsProccess();
    startDSProcess();
    inittellDialog();
    initReplyDialog();
    DSChatFlag = 0;
    tellOutputLimit(DSChatFlag);

    connect(scanTimer, &QTimer::timeout, [this]()//连接扫描定时器
        {
            if (DSChatFlag)
            {
                tellForm("0");
            }
        });

    connect(replyDialogHideTimer, &QTimer::timeout, [this]()//连接回复窗口定时器
        {
            replyDialog->hide();
            replyDialogHideTimer->stop();
        });

    scanTimer->start(1800000);//启动定时器
}

AronaDeepSeekProcess::~AronaDeepSeekProcess()
{
    aronaDeepSeekProcessKillFlag = 1;
    delete DSprocess;
}

void AronaDeepSeekProcess::initDsProccess()
{
    if ((arona->aronaData->deepseekAccout == "" || arona->aronaData->deepseekPassword == "")
        || (arona->aronaData->dsInitWord == "" && arona->aronaData->deepseekUrl == ""))
    {
        QMessageBox::information(AronaMainWindow, "Arona", "缺少DS聊天必要资源，DS将不会启动");
        return;
    }

    //传递初始化参数
    this->DSAccount = arona->aronaData->deepseekAccout;
    this->DSPassword = arona->aronaData->deepseekPassword;
    if (arona->aronaData->deepseekUrl == "")
        this->DSinitFlag = 0;
    else
        this->DSinitFlag = 1;
    this->DSUrl = arona->aronaData->deepseekUrl;
    this->DSInitWord = arona->aronaData->dsInitWord;
    //qDebug() << arona->aronaData->dsInitWord;////////////////////////////////
    if (arona->aronaData->dsThink)
        this->DSThink = "1";
    if (arona->aronaData->dsSearch)
        this->DSSearch = "1";

    //初始化正则匹配///////////////////////////////////////////////////////////////////////////////////
    replyAction.setPattern("/action\\{([^}]*)\\}");
    replyScanTime.setPattern("/scantime\\{([^}]*)\\}");
    replyReply.setPattern("/reply\\{([^}]*)\\}");
    replyReplyJP.setPattern("/replyJP\\{([^}]*)\\}");
}

//启动子进程
void AronaDeepSeekProcess::startDSProcess()
{
    DSprocess = new QProcess(this);
    connect(DSprocess, &QProcess::readyReadStandardOutput, this, &AronaDeepSeekProcess::onReadyRead);//连接读取
    connect(this, &AronaDeepSeekProcess::DSChatFlagChanged, this, &AronaDeepSeekProcess::tellOutputLimit);//连接输入限制状态变更
    DSprocess->start("deepseekSelenium.exe");//启动脚本
    DSstate = StateSendingAccount;//设置状态机 等待输入账号

    connect(DSprocess, &QProcess::stateChanged,
        this, [this](QProcess::ProcessState newState)
        {
            if (newState == QProcess::NotRunning)
                if (!aronaDeepSeekProcessKillFlag)
                    QMessageBox::warning(arona, "Arona", QString::fromLocal8Bit("deepseek process was unexpectedly closed"));
        });
}

//读取子进程输入
void AronaDeepSeekProcess::onReadyRead()
{
    DSlineIn = QString::fromUtf8(DSprocess->readAllStandardOutput()).trimmed();//读取数据
    //qDebug() << DSlineIn;/////////////////////////////////////////////////////////////////////////////////////////////
    if (!DSlineIn.isEmpty())
        DSinputFlag = 1;
}
//发送数据
void AronaDeepSeekProcess::sendDSData(QString& dataLine)
{
    //qDebug() << "\n\ntytytytytyyttttttttttttttttttttttttyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyttttttttttttttttyyyyyyyyyyyyyyyyyyyyyyy\n\n";////////////////////////////////////////////
    if (dataLine == "")
        return;
    //qDebug() << dataLine;////////////////////////////////////////////////////////////////////////////////////////////
    DSprocess->write((dataLine + '\n').toUtf8());
}

//限制对话窗口输出
void AronaDeepSeekProcess::tellOutputLimit(bool& newchatFlag)
{
    if (newchatFlag)//newchatFlag为1，允许输入
    {
        lineOut->setEnabled(1);
        tellLine->setEnabled(1);
        tellLine->setText("");
    }
    else
    {
        lineOut->setEnabled(0);
        tellLine->setEnabled(0);
        tellLine->setText("现在不能输入哦~~");
    }
}

//对话输出格式化打包
void AronaDeepSeekProcess::tellForm(QString actionModel)
{
    QString outline = tellLine->text();
    QString dateTime = QDateTime::currentDateTime().toString("yyyy,MM,dd,hh,mm,ss,ddd");//当前时间
    //构造输出文本
    QString telloutline_1 = "/time{";
    QString telloutline_2 = "}/action{";
    QString telloutline_3 = "}/tell{";
    QString telloutline_4 = "}";
    QString telloutline = telloutline_1 + dateTime + telloutline_2 + actionModel + telloutline_3 + outline + telloutline_4;
    //输出
    DSlineOut = telloutline;
    DSoutputFlag = 1;
}
//发送对话按键按下
void AronaDeepSeekProcess::on_lineOut_clicked()
{
    tellForm("1");
    tellLine->setText("");
    tellDialog->hide();
}
//初始化对话输出窗口
void AronaDeepSeekProcess::inittellDialog()
{
    if (!tellDialog)
    {
        //创建窗口
        tellDialog = new QDialog(AronaMainWindow, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Popup);//无边框，置顶，窗口自动关闭
        tellDialog->setAttribute(Qt::WA_TranslucentBackground);//窗口透明
        tellDialog->setFixedSize(400, 30);//设置窗口大小

        //创建对话框
        tellLine = new QLineEdit(tellDialog);
        tellLine->setGeometry(0, 0, 370, 30);

        //创建发送键
        lineOut = new QPushButton(tellDialog);
        lineOut->setGeometry(370, 0, 30, 30);
        lineOut->setIcon(QIcon(":/Arona/tellpushbutton.png"));//加载图片
        lineOut->setIconSize(QSize(30, 30));
        connect(lineOut, &QPushButton::pressed, this, &AronaDeepSeekProcess::on_lineOut_clicked);//连接按钮
    }
}
//显示对话输出窗口
void AronaDeepSeekProcess::showtellDialog()
{
    tellDialog->move(this->arona->pos() + QPoint(0, arona->aronaData->originalSize.height() * ((double)arona->aronaData->originalScale / 100.0) - 30));//使对话框出现在桌宠正下方
    tellDialog->show();
    tellDialog->activateWindow();
    tellLine->setFocus();
    //if (replyDialog) replyDialog->hide();////////////////////////////////////////////////
}
//初始化回复窗口
void AronaDeepSeekProcess::initReplyDialog()
{
    if (!replyDialog)
    {
        //创建窗口
        replyDialog = new QDialog(AronaMainWindow, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Dialog);//无边框，置顶
        replyDialog->setAttribute(Qt::WA_TranslucentBackground);//窗口透明
        //replyDialog->setFixedSize(200, 100);//设置窗口大小

        //创建对话框
        replyLine = new QTextEdit(replyDialog);
        replyLine->setReadOnly(true);// 只读模式
        replyLine->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);// 让文本在指定宽度内自动换行
        replyLine->setFrameStyle(QFrame::NoFrame);
        replyLine->setFixedWidth(replyLineMaxWidth);

        ////创建对话框
        //replyLine = new QPlainTextEdit(replyDialog);
        ////replyLine->setGeometry(0, 0, 200, 10000);
        //replyLine->setReadOnly(true); // 只读模式
        //replyLine->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);// 让文本在指定宽度内自动换行
        ////replyLine->setWordWrapMode(QTextOption::WrapAnywhere);

        //创建垂直布局，便于修改replyLine尺寸
        //QVBoxLayout* layout = new QVBoxLayout(replyDialog);
        //layout->setContentsMargins(0, 0, 0, 0);
        //layout->addWidget(replyLine);//添加replyLine

        //初始化replyLineSizeDoc
        //replyLineSizeDoc = new QTextDocument;
    }
}
//显示回复框
void AronaDeepSeekProcess::showReplyDialog()
{
    QApplication::processEvents();
    replyDialog->move(this->arona->pos() - QPoint(replyLineMaxWidth - 75, 0));
    replyDialog->show();

    //QTimer::singleShot(0, this, &AronaDeepSeekProcess::adjustReplyDialogSize);
    adjustReplyDialogSize();
}
//回复窗口自适应
void AronaDeepSeekProcess::adjustReplyDialogSize()
{
    //调整窗口宽度
    replyLine->setFixedWidth(replyLineMaxWidth);
    replyLine->document()->setTextWidth(replyLine->viewport()->width());

    // 3. 强制文档重新计算布局
    replyLine->document()->adjustSize();
    QApplication::processEvents();   // 等待布局完成

    //qDebug() <<"0d0007210d0007210d0007210d0007210d0007210d000721"<< replyLineMaxWidth + 3 << ' ' << replyLine->document()->size().toSize().height() + replyLine->frameWidth() * 2 + 2;
    //调整高度
    replyLine->setFixedSize(replyLineMaxWidth + 3, replyLine->document()->size().toSize().height() + 2);
    replyDialog->resize(replyLineMaxWidth + 3, replyLine->document()->size().toSize().height() + 2);
}
//分析回复
void AronaDeepSeekProcess::analyseReply(QString replyText)
{
    replyActionMatch = replyAction.match(replyText);

    if (replyActionMatch.hasMatch())
    {
        //对/action内容进行逐位分析

        if (replyActionMatch.captured(1).size() - 1 >= 0)
        {
            if (replyActionMatch.captured(1).mid(replyActionMatch.captured(1).size() - 1, 1) != '0')//查找第一位，确定是否有输出
            {
                //正则匹配回复内容
                replyReplyMatch = replyReply.match(replyText);
                replyReplyJPMatch = replyReplyJP.match(replyText);

                if (replyReplyMatch.hasMatch())
                {
                    //qDebug() << "0d0007210d0007210d0007210d000721" << replyReplyMatch.captured(1) << ' ' << replyReplyJPMatch.captured(1);///////////////////////////////////
                    replyCN = replyReplyMatch.captured(1);
                    replyJP = replyReplyJPMatch.captured(1);
                    replyLine->setText(replyCN);
                    showReplyDialog();//显示回复
                    replyDialogHideTimer->stop();
                    //qDebug() << "0d0007210d0007210d0007210d000721" << (replyCN.size() * 200 > 5000 ? replyCN.size() * 200 : 5000);//设置自动关闭时间;///////////////////////////////////
                    if (arona->aronaData->GPTopenFlag)
                        arona->aronaGPTosVITSProcess->TTSTextAnalyse(replyJP, "ja", replyDialog);//生成语音
                    else
                        replyDialogHideTimer->start(replyCN.size() * 200 > 5000 ? replyCN.size() * 200 : 5000);//设置自动关闭时间
                }
            }
        }
         
        if (replyActionMatch.captured(1).size() - 2 >= 0)
        {
            if (replyActionMatch.captured(1).mid(replyActionMatch.captured(1).size() - 2, 1) != '0')//查找第二位，确定是否修改扫描时间
            {
                //正则匹配扫描时间 
                replyScanTimeMatch = replyScanTime.match(replyText);
                if (replyScanTimeMatch.hasMatch())
                {
                    if (replyScanTimeMatch.captured(1) != "")
                    {
                        //qDebug() << "114514191985011451419198501145141919850114514191985011451419198501145141919850" << replyScanTimeMatch.captured(1) <<' ' << replyScanTimeMatch.captured(1).toInt() * 60000;/////////////////////////////////////////////////////////////
                        scanTimer->stop();
                        scanTimer->start(replyScanTimeMatch.captured(1).toInt() * 60000);//修改扫描时间
                    }
                }
            }
        }

            
    }
}

//状态转移和输入分析
void AronaDeepSeekProcess::analyseOutput()
{
    
    switch (DSstate)
    {
    case StateSendingAccount:
        sendDSData(DSAccount);
        DSstate = StateWaitingNumberOver;
        break;

    case StateWaitingNumberOver:
        if (DSinputFlag)
        {
            if (DSlineIn == "phoneNumberOver")
                DSstate = StateSendingPassword;
            DSlineIn = "";
            DSinputFlag = 0;
        }
        break;

    case StateSendingPassword:
        sendDSData(DSPassword);
        DSstate = StateWaitingPassWordOver;
        break;

    case StateWaitingPassWordOver:
        if (DSinputFlag)
        {
            if (DSlineIn == "passWordOver")
                DSstate = StateWaitingLogin;
            DSlineIn = "";
            DSinputFlag = 0;
        }
        break;

    case StateWaitingLogin:
        if (DSinputFlag)
        {
            if (DSlineIn == "loginSuc")
                DSstate = StateSendingThinkFlag;
            if (DSlineIn == "loginError")
            {
                DSprocess->close();
                QMessageBox::critical(qobject_cast<QWidget*>(parent()), "Arona", "登录失败");
                DSstate = StateIdle;
            }
            DSlineIn = "";
            DSinputFlag = 0;
        }
        break;

    case StateSendingThinkFlag:
        sendDSData(DSThink);
        DSstate = StateWaitingThinkFlagOver;
        break;

    case StateWaitingThinkFlagOver:
        if (DSinputFlag)
        {
            if (DSlineIn == "thinkFlagOver")
                DSstate = StateSendingSearchFlag;
            DSlineIn = "";
            DSinputFlag = 0;
        }
        break;

    case StateSendingSearchFlag:
        sendDSData(DSSearch);
        DSstate = StateWaitingSearchFlagOver;
        break;

    case StateWaitingSearchFlagOver:
        if (DSinputFlag)
        {
            if (DSlineIn == "searchFlagOver")
                DSstate = StateSendingInit;
            DSlineIn = "";
            DSinputFlag = 0;
        }
        break;

    case StateSendingInit:
        if (DSinitFlag)
            sendDSData(DSninit);
        else
            sendDSData(DSinit);
        DSstate = StateWaitingInit;
        break;

    case StateWaitingInit:
        if (DSinputFlag)
        {
            if (DSlineIn == "initOver")
            {
                if (DSinitFlag)
                    DSstate = StateSendingUrl;
                else
                    DSstate = StateSendingInitText;
            }
            DSlineIn = "";
            DSinputFlag = 0;
        }
        break;

    case StateSendingInitText:
        sendDSData(DSInitWord);
        DSstate = StateWaitingChatUrl;
        break;

    case StateWaitingChatUrl:
        if (DSinputFlag)
        {
            DSUrl = DSlineIn;
            //emit DSUrlChanged(DSUrl);//发送Url修改信号
            this->arona->aronaData->deepseekUrl = DSUrl;
            DSstate = StateWaitingGPTsoVITSInit;
            DSChatFlag = 1;
            emit DSChatFlagChanged(DSChatFlag);
            DSlineIn = "";
            DSinputFlag = 0;
        }
        break;

    case StateSendingUrl:
        sendDSData(DSUrl);
        DSstate = StateWaitingInitChat;
        break;

    case StateWaitingInitChat:
        if (DSinputFlag)
        {
            if (DSlineIn == "initChatOver")
            {
                DSstate = StateSendingReCommand;
                //DSChatFlag = 1;
                //emit DSChatFlagChanged(DSChatFlag);
            }
            if (DSlineIn == "pointlessUrl")
            {
                DSprocess->close();
                QMessageBox::critical(qobject_cast<QWidget*>(parent()), "Arona", "无效网址");
                DSstate = StateIdle;
            }
            DSlineIn = "";
            DSinputFlag = 0;
        }
        break;
    case StateSendingReCommand:
        DSChatFlag = 1;
        emit DSChatFlagChanged(DSChatFlag);
        tellForm("0");
        DSlineOut = DSlineOut + "/recommand{" + DSInitWord + "}";
        //qDebug() << DSInitWord;
        sendDSData(DSlineOut);
        DSChatFlag = 0;
        emit DSChatFlagChanged(DSChatFlag);
        DSlineOut = "";
        DSoutputFlag = 0;
        DSstate = StateWaitingReCommandReply;
        break;

    case StateWaitingReCommandReply:
        if (DSinputFlag)
        {
            DSstate = StateWaitingGPTsoVITSInit;
            DSChatFlag = 1;
            emit DSChatFlagChanged(DSChatFlag);
            //DSlineIn = "";
            //DSinputFlag = 0;
        }
        break;

    case StateSendingChatOutput:
        if (DSoutputFlag)
        {
            sendDSData(DSlineOut);
            DSlineOut = "";
            DSoutputFlag = 0;
            DSstate = StateWaitingChatInput;
            DSChatFlag = 0;
            emit DSChatFlagChanged(DSChatFlag);
        }
        break;

    case StateWaitingChatInput:
        if (DSinputFlag)
        {
            //输入缓冲在主程序接收并清除
            if (DSlineIn == "exitOver")
                DSstate = StateExiting;
            else
            {
                DSChatFlag = 1;
                emit DSChatFlagChanged(DSChatFlag);
                DSstate = StateSendingChatOutput;
                //DSinputFlag = 0;
                //DSlineIn = "";
            }
        }
        break;

    case StateExiting:
        DSChatFlag = 0;
        emit DSChatFlagChanged(DSChatFlag);
        DSprocess->close();
        break;

    case StateWaitingGPTsoVITSInit:
        if (!arona->aronaData->GPTopenFlag || arona->aronaGPTosVITSProcess->aronaGPTsoVITSProcessReadyFlag)
            DSstate = StateSendingChatOutput;

    default:
        break;
    }
}
