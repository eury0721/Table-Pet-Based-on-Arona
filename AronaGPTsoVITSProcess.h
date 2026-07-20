#ifndef _AronaGPTsoVITSProcess_h_
#define _AronaGPTsoVITSProcess_h_

#include <QProcess>
#include <QDir>
#include <QCoreApplication>
#include <QFile>
//#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <qmessagebox.h>
#include <qdialog.h>
#include <qstringlist.h>
#include <QApplication>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QRegularExpression>

class Arona;

class tempTTStext//临时存储TTS用到的数据
{
public:
	QString text;
	QString language;
	QDialog* dialog;
};

class AronaGPTsoVITSProcess : public QObject
{
	Q_OBJECT
public:
	AronaGPTsoVITSProcess(QWidget* parent = nullptr);
	~AronaGPTsoVITSProcess();

	//父指针
	QWidget* AronaMainWindow = nullptr;
	Arona* arona = nullptr;

	QProcess* aronaGPTsoVITSprocess;//GPTsoVITS启动和运行的的进程
	QNetworkAccessManager* aronaGPTsoVITSNetworkManage;//GPTsoVITS运行管理器

	bool aronaGPTsoVITSProcessReadyFlag = 0;//GPTsoVITSapi进程准备标志位
	bool aronaGPTsoVITSProcessKillFlag = 0;//GPTsoVITSapi进程关闭标志位
	bool TTSrestFlag = 0;//TTS序列变更标志位(顶替现有序列)

	bool unreadyTTSTextFlag = 0;//在启动api前有TTS请求标志
	tempTTStext unreadyTTS;//在启动api前TTS请求数据

	bool onTTSFlag = 0;//正在进行TTS标志
	QStringList witiringTTSText;//等待TTS的文本列表，首项为正在进行TTS的文本
	QString TTSlanguage;//TTS文本的语言
	int TTSOutputFileName = 0;//输出文件名称，使用时转为QString
	QStringList witiringPlayFile;//等待播放的音频文件列表
	QDialog* TTSTextDialog;//文本所在窗口
	QRegularExpression separator;//分割文本的依据

	QString tempSoundFileUrl;//在序列变更时保存API讲输出文件的路径
	tempTTStext restTTS;//在替换数据时存储数据

	QMediaPlayer* player;//播放器
	QAudioOutput* audioOutput;//音频输出

	void startGPTsoVITSapi();//启动GPTsoVITSapi
	void GPTsoVITSTTS(QString TTSText, QString TTSTextLanguage, QString soundOutFileUrl);//生成TTS语音
	void TTSTextAnalyse(QString text, QString TTSTextLanguage, QDialog* textGialog);//分析TTS文本，控制文本窗口显示

	void nextTTS();//顺次进行TTS

	void GPTsoVITSapiClose();//关闭api

public slots:
	void aronaGPTsoVITSProcessRead();//读取GPTsoVITS输出

	void aronaGPTsoVITSTTSReply(QNetworkReply* networkReply);//处理响应

	void nextplay(QMediaPlayer::MediaStatus status);//顺次播放

private:
	QString GPTsoVITSurl;//访问的地址
	std::string intToString(int num);

signals:
	void soundFileFinish();//音频文件输出完成标志
};



#endif // !_AronaGPTsoVITSProcess_h_
