#ifndef _Arona_H_
#define _Arona_H_

#include <QtWidgets/QMainWindow>
//#include <qdebug.h>
#include <qtimer.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qsize.h>
#include <qevent.h>
#include <qpoint.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qimagereader.h>
#include <qslider.h>
#include <qlabel.h>
#include <qsystemtrayicon.h>
#include "ui_Arona.h"
#include "ui_AronaSettings.h"
#include "AronaData.h"
#include "AronaDeepSeekProcess.h"
#include "AronaGPTsoVITSProcess.h"
//#include "AronaApplication.h"

QT_BEGIN_NAMESPACE
namespace Ui_settings
{
    class AronaSettings;
};
QT_END_NAMESPACE

class AronaSettings : public QDialog
{
    Q_OBJECT
public:
    explicit AronaSettings(QWidget* parent = nullptr);
    ~AronaSettings();

    //显示
    void aronaSettingsShow();
    //隐藏
    void aronaSettingsHide();

    Ui_settings::AronaSettings* ui;

signals:
    //确定信号
    void accepted();
    //取消信号
    void canceled();

private slots:
    //确定
    void on_settingAccept_clicked();
    //取消
    void on_settingCancel_clicked();
    //重置聊天
    void on_chatInit_clicked();
};


QT_BEGIN_NAMESPACE
namespace Ui
{
    class AronaClass;
};
QT_END_NAMESPACE


class AronaSizeSlider;//大小调整滑块
class AronaMenu;//菜单
class AronaTrayTcon;//系统托盘


class Arona : public QMainWindow
{
    Q_OBJECT

public:
    Arona(QWidget* parent = nullptr);
    ~Arona();

    AronaData* aronaData = nullptr;//指向储存数据的指针
    //AronaSizeSlider* aronaSizeSlider = nullptr;//指向缩放滑块的指针
    AronaMenu* aronaMenu = nullptr;//指向菜单的指针
    AronaTrayTcon* aronaTrayIcon = nullptr;//指向系统托盘的指针
    AronaSettings* aronaSettings = nullptr;//指向设置窗口的指针
    AronaDeepSeekProcess* aronaDeepseekProcess = nullptr;//指向deepseek聊天脚本的指针
    AronaGPTsoVITSProcess* aronaGPTosVITSProcess = nullptr;//指向GPTsoVITSTTSapi的指针

    //显示设置窗口
    void aronaSettingsShow();

public slots:
    //菜单点击信号
    void contextMenuEvent(QContextMenuEvent* action);
    //系统托盘槽
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    //设置窗口确定信号
    void aronaSettingsAccept();
    //设置窗口取消信号
    void aronaSettingsCancel();

protected:
    //绘制帧
    void paintEvent(QPaintEvent* event) override;

private:
    Ui::AronaClass* ui;

    //计时器
    QTimer* frameTimer = nullptr;//帧计时器

    //绘制帧
    int expressType = 0;//储存表情类型
    int frameNumber = 0;//存储当前表情帧

    //刷新检测状态机
    void refreshDSprocess();


};
/*
* 本体主要职能为维持正常循环
* 单帧(->绘制帧->查询AI通话状态机->)
*/


class DragFilter :public QObject//鼠标拖动
{
public:
    //重写事件，实现鼠标拖动
    bool eventFilter(QObject* obj, QEvent* event);

private:
    QPoint pos;//鼠标与窗口的相对位置，记录窗口相对偏移量
};



#endif // !_Arona_H_