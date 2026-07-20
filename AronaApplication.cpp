#include "Arona.h"
#include "AronaApplication.h"

std::string intToString(int num)
{
    std::string st = "";
    while (num)
    {
        st = (char)(num % 10 + '0') + st;
        num /= 10;
    }
    return st;
}

AronaSizeSlider::AronaSizeSlider(QWidget* parent)
    :AronaMainWindow(parent)
{
    arona = qobject_cast<Arona*>(AronaMainWindow);
    initSizeSlider();
}

AronaSizeSlider::~AronaSizeSlider()
{
    delete sizeLabel;
    delete sizeSlider;
    delete sliderDialog;
}

//处理滑块
void AronaSizeSlider::onSizeSliderChanged(int value)
{
    //更新缩放
    arona->aronaData->originalScale = value;
    arona->resize(arona->aronaData->originalSize.width() * ((double)arona->aronaData->originalScale / 100.0), arona->aronaData->originalSize.height() * ((double)arona->aronaData->originalScale / 100.0));

    //更新标签显示
    if (sizeLabel)
    {
        sizeLabel->setText(QString::fromStdString(intToString(arona->aronaData->originalScale) + '%'));
    }
}

//初始化滑块
void AronaSizeSlider::initSizeSlider()
{
    if (!sliderDialog)
    {
        //创建窗口
        sliderDialog = new QDialog(this, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Popup);//无边框，置顶，窗口自动关闭
        sliderDialog->setAttribute(Qt::WA_TranslucentBackground);//窗口透明
        sliderDialog->setFixedSize(60, 200);//设置大小

        //创建滑块
        sizeSlider = new QSlider(Qt::Vertical, sliderDialog);
        sizeSlider->setRange(10, 200);//设置滑块范围
        sizeSlider->setValue(arona->aronaData->originalScale);//设置初始值
        //sizeSlider->setSingleStep(10);//设置步距
        //sizeSlider->setTickPosition(QSlider::TicksBothSides);//
        //sizeSlider->setTickInterval(10);//
        sizeSlider->setGeometry(10, 20, 20, 160);//设置滑块位置

        //创建便签显示缩放比
        sizeLabel = new QLabel(QString::fromStdString(intToString(arona->aronaData->originalScale) + '%'), sliderDialog);
        sizeLabel->setAlignment(Qt::AlignCenter);//文字居中
        sizeLabel->setGeometry(0, 180, 60, 20);//设置便签位置
        sizeLabel->setStyleSheet("color: black; background: white; border: 1px solid gray;");//设置字体

        // 连接滑块
        connect(sizeSlider, &QSlider::valueChanged, this, &AronaSizeSlider::onSizeSliderChanged);
    }
}

//显示化滑块
void AronaSizeSlider::showSizeSlider()
{
    // 将滑块窗口定位到鼠标位置附近
    QPoint pos = QCursor::pos();
    sliderDialog->move(pos - QPoint(30, 100)); // 偏移使滑块在鼠标上方
    sliderDialog->show();
}


AronaMenu::AronaMenu(QWidget* parent)
	:AronaMainWindow(parent)
    ,menu(new QMenu(parent))
    ,aronaSizeSlider(new AronaSizeSlider(parent))
{
    arona = qobject_cast<Arona*>(AronaMainWindow);
    initMenu();
}

AronaMenu::~AronaMenu()
{
    delete menu;
}

void AronaMenu::onMenuTtiggerd(QAction* action)
{
    QString text = action->text();
    if (text == "显示窗口")
    {
        Qt::WindowFlags windowTile = arona->windowFlags();
        windowTile &= ~Qt::FramelessWindowHint;
        arona->setWindowFlags(windowTile);
        arona->show();
    }
    if (text == "隐藏窗口")
    {
        Qt::WindowFlags windowTile = arona->windowFlags();
        windowTile |= Qt::FramelessWindowHint;
        arona->setWindowFlags(windowTile);
        arona->show();
    }
    if (text == "置顶窗口")
    {
        Qt::WindowFlags windowTop = arona->windowFlags();
        windowTop |= Qt::WindowStaysOnTopHint;
        arona->setWindowFlags(windowTop);
        arona->show();
    }
    if (text == "取消置顶")
    {
        Qt::WindowFlags windowTop = arona->windowFlags();
        windowTop &= ~Qt::WindowStaysOnTopHint;
        arona->setWindowFlags(windowTop);
        arona->show();
    }
    if(text == "设置")
    { 
        arona->aronaSettingsShow();
    }

    if (text == "隐藏桌宠")
        arona->setVisible(false);
    if (text == "显示桌宠")
        arona->setVisible(true);
    if (text == "聊天")
        arona->aronaDeepseekProcess->showtellDialog();
    if (text == "退出")
        qApp->quit();
}

//初始化菜单
void AronaMenu::initMenu()
{
    //menu->addAction("调整大小", this, &AronaSizeSlider::showSizeSlider);
    QAction* actSize = menu->addAction("调整大小");
    connect(actSize, &QAction::triggered, this, [this]() 
        {
            if (aronaSizeSlider) aronaSizeSlider->showSizeSlider();
        });
    //menu->addAction("聊天", this, &Arona::showtellDialog);
    Arona* AronaMainWindow = this->arona;
    menu->addAction("隐藏窗口");
    menu->addAction("显示窗口");
    menu->addAction("置顶窗口");
    menu->addAction("取消置顶");
    menu->addAction("隐藏桌宠");
    menu->addAction("显示桌宠");
    menu->addAction("聊天");
    menu->addAction("设置");

    QAction* actExit = new QAction("退出");
    connect(actExit, &QAction::triggered, [AronaMainWindow]()
        {
            qApp->quit();
        });
    menu->addAction("退出");

    connect(this->menu, &QMenu::triggered, this, &AronaMenu::onMenuTtiggerd);
}

AronaTrayTcon::AronaTrayTcon(QWidget* parent)
    :AronaMainWindow(parent)
    ,trayIcon(new QSystemTrayIcon(parent))
{
    arona = qobject_cast<Arona*>(AronaMainWindow);
    initTrayIcon();
}

AronaTrayTcon::~AronaTrayTcon()
{
    trayIcon->hide();
    delete trayIcon;
}

//初始化系统托盘
void AronaTrayTcon::initTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);//创建托盘图标
    trayIcon->setIcon(QIcon("Arona.ico"));//设置图标
    trayIcon->setToolTip("Arona");//设置悬停提示文本
    trayIcon->setContextMenu(arona->aronaMenu->menu);//共享已有的右键菜单
    trayIcon->show();// 显示托盘图标

    connect(trayIcon, &QSystemTrayIcon::activated, arona, &Arona::onTrayIconActivated);//连接托盘图标的激活信号
}

