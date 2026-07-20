#include "Arona.h"
#include "AronaApplication.h"

Arona::Arona(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AronaClass())
    , frameTimer(new QTimer(this))
    , aronaData(new AronaData(this))
    //, aronaSizeSlider(new AronaSizeSlider(this))
    , aronaMenu(new AronaMenu(this))
    , aronaTrayIcon(new AronaTrayTcon(this))
    , aronaSettings(new AronaSettings(this))
    , aronaDeepseekProcess(new AronaDeepSeekProcess(this))
    , aronaGPTosVITSProcess(new AronaGPTsoVITSProcess(this))
{
    //设置主窗口属性
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);//除去边框与置顶
    this->setAttribute(Qt::WA_TranslucentBackground);//去除背景
    this->installEventFilter(new DragFilter);//启用窗口拖动
    this->setWindowIcon(QIcon("Arona.ico"));//设置图标

    //连接槽函数
    connect(aronaSettings, &AronaSettings::accepted,this, &Arona::aronaSettingsAccept);//设置确定
    connect(aronaSettings, &AronaSettings::canceled, this, &Arona::aronaSettingsCancel);//设置取消

    //启动GPT-soVITS
    if (aronaData->GPTopenFlag)
    {
        aronaGPTosVITSProcess->startGPTsoVITSapi();
    }

    //初始化
    expressType = 0;
    connect(frameTimer, &QTimer::timeout, [this]()//连接帧定时器
        {
            frameNumber = (frameNumber + 1) % aronaData->bodyExpression[aronaData->expressName[expressType]].size();//向下迭代迭代器，更新帧
            this->update();
            refreshDSprocess();
            //if (aronaGPTosVITSProcess->aronaGPTsoVITSprocess->state() == QProcess::Running)
            //{
            //    qDebug() << "API Running";
            //}
            //else if (aronaGPTosVITSProcess->aronaGPTsoVITSprocess->state() == QProcess::NotRunning) 
            //    qDebug() << "API NoRunning";
        });
    frameTimer->start(33);//启用帧定时器，设置为30帧


    ui->setupUi(this);//必须在setupUi后resize
    this->resize(aronaData->bodyExpression[aronaData->expressName[expressType]][frameNumber].size().width() * ((double)aronaData->originalScale / 100.0)
        , aronaData->bodyExpression[aronaData->expressName[expressType]][frameNumber].size().height() * ((double)aronaData->originalScale / 100.0));//设定窗口大小,自适应帧图片大小
    aronaData->originalSize = aronaData->bodyExpression[aronaData->expressName[expressType]][frameNumber].size();//记录原窗口大小
}

Arona::~Arona()
{
    delete ui;
    delete frameTimer;
    //delete aronaSizeSlider;
    delete aronaMenu;
    delete aronaTrayIcon;
    delete aronaSettings;
    delete aronaData;
    delete aronaDeepseekProcess;
    delete aronaGPTosVITSProcess;
}

//绘制帧
void Arona::paintEvent(QPaintEvent* evevnt)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0
        , aronaData->bodyExpression[aronaData->expressName[expressType]][frameNumber].scaled(aronaData->originalSize.width() * ((double)aronaData->originalScale / 100.0)
        , aronaData->originalSize.height() * ((double)aronaData->originalScale / 100.0)
        , Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// 连接至aronaMenu里，直接弹出QMenu
void Arona::contextMenuEvent(QContextMenuEvent* event)
{
    aronaMenu->menu->popup(event->globalPos());
    event->accept();
}

//处理托盘信息
void Arona::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)//左键点击
    {
        if (!this->isVisible())//窗口不显示
        {
            this->show();//显示窗口
            this->raise();//置顶窗口
            this->activateWindow();//激活窗口
        }
    }
}

//显示设置窗口
void Arona::aronaSettingsShow()
{
    aronaSettings->aronaSettingsShow();//显示
    //传递设置数据
    aronaSettings->ui->DSaccount->setText(aronaData->deepseekAccout);
    aronaSettings->ui->DSpassword->setText(aronaData->deepseekPassword);
    aronaSettings->ui->UrlSetttings->setText(aronaData->deepseekUrl);
    aronaSettings->ui->GPTopenFlag->setChecked(aronaData->GPTopenFlag);
    aronaSettings->ui->SearchFlag->setChecked(aronaData->dsSearch);
    aronaSettings->ui->ThinkFlag->setChecked(aronaData->dsThink);
}

//设置窗口确定信号
void Arona::aronaSettingsAccept()
{
    //传回设置数据
    aronaData->deepseekAccout = aronaSettings->ui->DSaccount->text();
    aronaData->deepseekPassword = aronaSettings->ui->DSpassword->text();
    aronaData->deepseekUrl = aronaSettings->ui->UrlSetttings->text();
    aronaData->GPTopenFlag = aronaSettings->ui->GPTopenFlag->checkState();
    aronaData->dsSearch = aronaSettings->ui->SearchFlag->checkState();
    aronaData->dsThink = aronaSettings->ui->ThinkFlag->checkState();
}
//设置窗口取消信号
void Arona::aronaSettingsCancel()
{
    //重置设置数据
    aronaSettings->ui->DSaccount->setText(aronaData->deepseekAccout);
    aronaSettings->ui->DSpassword->setText(aronaData->deepseekPassword);
    aronaSettings->ui->UrlSetttings->setText(aronaData->deepseekUrl);
    aronaSettings->ui->GPTopenFlag->setChecked(aronaData->GPTopenFlag);
    aronaSettings->ui->SearchFlag->setChecked(aronaData->dsSearch);
    aronaSettings->ui->ThinkFlag->setChecked(aronaData->dsThink);
}


//鼠标拖动桌宠
bool DragFilter::eventFilter(QObject* obj, QEvent* event)
{
    auto AronaWindow = dynamic_cast<QWidget*>(obj);
    if (!AronaWindow)
    {
        return 0;
    }
    if (event->type() == QEvent::MouseButtonPress)//鼠标按下，记录相对偏移量
    {
        auto Event = dynamic_cast<QMouseEvent*>(event);
        if (Event)
        {
            pos = Event->pos();
            return 1;
        }
    }
    else
    {
        if (event->type() == QEvent::MouseMove)//鼠标移动
        {
            auto Event = dynamic_cast<QMouseEvent*>(event);
            if (Event)
            {
                if (Event->buttons() & Qt::MouseButton::LeftButton)//先判断移动，后判断暗下，防止误触
                {
                    AronaWindow->move(Event->globalPosition().toPoint() - pos);//求偏移并移动
                    return 1;
                }
            }
        }
    }
    return QObject::eventFilter(obj, event);
}

//设置窗口构造函数
AronaSettings::AronaSettings(QWidget* parent) 
    : QDialog(parent)
    , ui(new Ui_settings::AronaSettings())
{
    ui->setupUi(this);
}
//设置窗口析构函数
AronaSettings::~AronaSettings()
{

}

//显示设置窗口
void AronaSettings::aronaSettingsShow()
{
    this->show();
}
//隐藏设置窗口
void AronaSettings::aronaSettingsHide()
{
    this->hide();
}

//确定
void AronaSettings::on_settingAccept_clicked()
{
    emit accepted();//发射确定信号
}
//取消
void AronaSettings::on_settingCancel_clicked()
{
    emit canceled();//发射取消信号
}
//重置聊天
void AronaSettings::on_chatInit_clicked()
{
    ui->UrlSetttings->setText("");
}

//刷新检测状态机
void Arona::refreshDSprocess()
{
    this->aronaDeepseekProcess->analyseOutput();
    //if (this->aronaDeepseekProcess->DSChatFlag)
    //{
        if (this->aronaDeepseekProcess->DSinputFlag)
        {
            //this->aronaDeepseekProcess->replyLine->setPlainText(this->aronaDeepseekProcess->DSlineIn);//输入文本至输出框
            //this->aronaDeepseekProcess->showReplyDialog();//显示输出
            this->aronaDeepseekProcess->analyseReply(this->aronaDeepseekProcess->DSlineIn);//分析回复内容
            this->aronaDeepseekProcess->DSlineIn = "";
            this->aronaDeepseekProcess->DSinputFlag = 0;
        }
    //}
}