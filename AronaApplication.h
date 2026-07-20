#ifndef _AronaApplication_H_
#define _AronaApplication_H_

#include <qwidget.h>
#include <qobject.h>
#include <qdialog.h>
#include <qapplication.h>
#include <qimagereader.h>
#include <qslider.h>
#include <qcursor.h>
#include <qlabel.h>
#include <qevent.h>
#include <qmenu.h>
#include <qsystemtrayicon.h>

class Arona;

class AronaSizeSlider : public QDialog
{
	Q_OBJECT
public:
	AronaSizeSlider(QWidget* parent);
	~AronaSizeSlider();
	QWidget* AronaMainWindow = nullptr;
	Arona* arona = nullptr;

	QDialog* sliderDialog = nullptr;//弹出滑块的窗口
	QSlider* sizeSlider = nullptr;//大小滑块
	QLabel* sizeLabel = nullptr;//显示缩放比

	//显示化滑条
	void showSizeSlider();

private:

	//处理滑块
	void onSizeSliderChanged(int value);
	//初始化滑条
	void initSizeSlider();
	
};


class AronaMenu : public QWidget //菜单
{
	Q_OBJECT
public:
	AronaMenu(QWidget* parent = nullptr);
	~AronaMenu();
	QWidget* AronaMainWindow = nullptr;
	Arona* arona = nullptr;

	AronaSizeSlider* aronaSizeSlider = nullptr;//指向缩放滑块的指针
	QMenu* menu = nullptr;//菜单
	/*
	* 调整大小
	* 聊天
	* 隐藏窗口
	* 显示窗口
	* 置顶桌宠
	* 取消置顶
	* 隐藏桌宠
	* 显示桌宠
	* 设置
	* 退出
	*/

//public slots:
//	//菜单槽
//	void contextMenuEvent(QContextMenuEvent* event);

private:

	//菜单点击信号
	void onMenuTtiggerd(QAction* action);
	//初始化菜单
	void initMenu();

};

class AronaTrayTcon : public QWidget//系统托盘
{
	Q_OBJECT
public:
	AronaTrayTcon(QWidget* parent = nullptr);
	~AronaTrayTcon();
	QWidget* AronaMainWindow = nullptr;
	Arona* arona = nullptr;
	QSystemTrayIcon* trayIcon = nullptr;//系统托盘

private:
	//初始化系统托盘
	void initTrayIcon();

};



#endif // !_AronaApplication_H_
