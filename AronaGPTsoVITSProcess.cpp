#include "Arona.h"
#include "AronaGPTsoVITSProcess.h"

AronaGPTsoVITSProcess::AronaGPTsoVITSProcess(QWidget* parent)
	: AronaMainWindow(parent)
	, aronaGPTsoVITSprocess(new QProcess(this))
	, aronaGPTsoVITSNetworkManage(new QNetworkAccessManager(this))
	, player(new QMediaPlayer)
	, audioOutput(new QAudioOutput)
{
	arona = qobject_cast<Arona*>(AronaMainWindow);

	separator.setPattern("[。！？\n.!?]");

	player->setAudioOutput(audioOutput);
	audioOutput->setVolume(0.8);
	connect(player, &QMediaPlayer::mediaStatusChanged, this, &AronaGPTsoVITSProcess::nextplay);//在播完一句后切下一句
}

AronaGPTsoVITSProcess::~AronaGPTsoVITSProcess()
{
	aronaGPTsoVITSProcessKillFlag = 1;
	GPTsoVITSapiClose();
	delete aronaGPTsoVITSprocess;
}

std::string AronaGPTsoVITSProcess::intToString(int num)
{
	std::string st = "";
	while (num)
	{
		st = (char)(num % 10 + '0') + st;
		num /= 10;
	}
	return st;
}

//启动GPTsoVITSapi
void AronaGPTsoVITSProcess::startGPTsoVITSapi()
{
	aronaGPTsoVITSprocess->setWorkingDirectory(".\\GPT-SoVITS");//转到GPTsoVITS位置
	connect(aronaGPTsoVITSprocess, &QProcess::readyReadStandardError, this, &AronaGPTsoVITSProcess::aronaGPTsoVITSProcessRead);//连接输出
	QStringList apiStartArguments;//输入参数
	//apiStartArguments << "runtime\\python.exe";
	apiStartArguments << "api.py";
	apiStartArguments << "-s" << arona->aronaData->soVITSWeightFile;
	apiStartArguments << "-g" << arona->aronaData->GPTWeightFile;
	apiStartArguments << "-a" << arona->aronaData->GPTsoVITSAddress;
	apiStartArguments << "-p" << arona->aronaData->GPTsoVITSPort;
	apiStartArguments << "-dr" << arona->aronaData->GPTsoVITSReferenceFile;
	apiStartArguments << "-dt" << arona->aronaData->GPTsoVITSReferenceText;
	apiStartArguments << "-dl" << arona->aronaData->GPTsoVITSReferenceLanguage;

	GPTsoVITSurl = "http://" + arona->aronaData->GPTsoVITSAddress + ":" + arona->aronaData->GPTsoVITSPort;//生成访问的地址

	aronaGPTsoVITSprocess->start(".\\GPT-SoVITS\\runtime\\python.exe", apiStartArguments);

	connect(aronaGPTsoVITSprocess, &QProcess::stateChanged,
		this, [this](QProcess::ProcessState newState)
		{
			if (newState == QProcess::NotRunning())
				if (!aronaGPTsoVITSProcessKillFlag)
					QMessageBox::warning(arona, "Arona", QString::fromLocal8Bit("GPTsoVITS process was unexpectedly closed"));
		});
	connect(aronaGPTsoVITSNetworkManage, &QNetworkAccessManager::finished,this, &AronaGPTsoVITSProcess::aronaGPTsoVITSTTSReply);//连接请求和响应
}

//生成TTS语音
void AronaGPTsoVITSProcess::GPTsoVITSTTS(QString TTSText, QString TTSTextLanguage, QString soundOutFileUrl)
{
	//生成请求
	QUrl GPTsoVITSUrl(GPTsoVITSurl);
	QNetworkRequest aronaGPTsoVITSTTSrequest(GPTsoVITSUrl);
	aronaGPTsoVITSTTSrequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QJsonObject payload;
	payload["text"] = TTSText;
	payload["text_language"] = TTSTextLanguage;

	QByteArray jsonData = QJsonDocument(payload).toJson();

	//qDebug().noquote() << "POST URL:" << GPTsoVITSUrl.toString();/////////////////////////////////////////////////////////////////////////////////////
	//qDebug().noquote() << "POST Data:" << QString::fromUtf8(jsonData);///////////////////////////////////////////////////////////////////////////////

	QNetworkReply* networkReply = aronaGPTsoVITSNetworkManage->post(aronaGPTsoVITSTTSrequest, jsonData);
	//将输出路径附加到 networkReply 对象上
	networkReply->setProperty("outputPath", soundOutFileUrl);
}

//处理响应
void AronaGPTsoVITSProcess::aronaGPTsoVITSTTSReply(QNetworkReply* networkReply)
{
	if (networkReply->error() == QNetworkReply::NoError)
	{
		// 保存为文件
		QFile file(networkReply->property("outputPath").toString());
		if (file.open(QIODevice::WriteOnly)) 
		{
			file.write(networkReply->readAll());// 传输waw音频数据
			file.close();
			witiringPlayFile.operator+=(networkReply->property("outputPath").toString());//保存文件地址
			if (!TTSrestFlag)
			{
				if (witiringPlayFile.size() == 1)//列表中仅当前文件，则播放
				{
					TTSTextDialog->show();
					player->setSource(QUrl::fromLocalFile(witiringPlayFile[0]));
					player->play();
				}
				nextTTS();
			}
			else//清除生成的文件，重新开始新序列
			{
				TTSrestFlag = 0;
				QFile::remove(networkReply->property("outputPath").toString());
				TTSTextAnalyse(restTTS.text, restTTS.language, restTTS.dialog);
			}
		}
	}

	networkReply->deleteLater();
}

//分析TTS文本，控制文本窗口显示
void AronaGPTsoVITSProcess::TTSTextAnalyse(QString text, QString TTSTextLanguage, QDialog* textGialog)
{
	textGialog->hide();
	if (aronaGPTsoVITSProcessReadyFlag)//api未启动
	{
		if (witiringTTSText.size() != 0)//正在进行TTS
		{
			TTSrestFlag = 1;//传递变更信息在当前正在TTS完成后重置任务

			tempSoundFileUrl = QString::fromStdString(intToString(TTSOutputFileName));
			tempSoundFileUrl += ".wav";

			//保存数据
			restTTS.text = text;
			restTTS.language = TTSTextLanguage;
			restTTS.dialog = textGialog;

			TTSTextDialog->hide();
			TTSOutputFileName = 0;
			TTSlanguage = "";
			witiringTTSText.clear();//清除待TTS的文本
			player->stop();//暂停正在播放的语音
			for (int i = 0; i < witiringPlayFile.size(); i++)
			{
				QFile::remove(witiringPlayFile[i]);//清除待播放的音频文件
			}
			witiringPlayFile.clear();//清除待播放的音频文件列表
		}
		else
		{
			TTSTextDialog = textGialog;
			witiringTTSText = text.split(separator, Qt::SkipEmptyParts);
			TTSOutputFileName = 0;
			TTSlanguage = TTSTextLanguage;
			GPTsoVITSTTS(witiringTTSText[0], TTSlanguage, QString::fromStdString(intToString(TTSOutputFileName) + ".wav"));
		}
	}
	else
	{
		unreadyTTS.text = text;
		unreadyTTS.language = TTSTextLanguage;
		unreadyTTS.dialog = textGialog;
		unreadyTTSTextFlag = 1;
	}
}

//顺次进行TTS
void AronaGPTsoVITSProcess::nextTTS()
{

	witiringTTSText.removeFirst();
	TTSOutputFileName++;

	if(!witiringTTSText.isEmpty())
		GPTsoVITSTTS(witiringTTSText[0], TTSlanguage, QString::fromStdString(intToString(TTSOutputFileName) + ".wav"));
}

//顺次播放
void AronaGPTsoVITSProcess::nextplay(QMediaPlayer::MediaStatus status)
{
	if (status != QMediaPlayer::EndOfMedia)
		return;

	//删除上次播放的文件
	QFile::remove(witiringPlayFile[0]);
	//qDebug() << witiringPlayFile[0];/////////////////////////////////////////////////////////////////////////////////////
	witiringPlayFile.removeFirst();

	//顺次播放
	if (!witiringPlayFile.isEmpty())
	{
		player->setSource(QUrl::fromLocalFile(witiringPlayFile[0]));
		player->play();
	}
	else
	{
		if (witiringTTSText.isEmpty())//播放完毕关闭
		{
			TTSTextDialog->hide();
		}
	}

}

//读取GPTsoVITS输出
void AronaGPTsoVITSProcess::aronaGPTsoVITSProcessRead()
{
	QString aronaGPTsoVITSProcessOut = aronaGPTsoVITSprocess->readAllStandardError();
	//qDebug() << "07210721072107210721" << aronaGPTsoVITSProcessOut;/////////////////////////////////////////////////////////////////

	if (aronaGPTsoVITSProcessOut == "INFO:     Application startup complete.\r\n")
	{
		aronaGPTsoVITSProcessReadyFlag = 1;
		if (unreadyTTSTextFlag)
		{
			unreadyTTSTextFlag = 0;
			TTSTextAnalyse(unreadyTTS.text, unreadyTTS.language, unreadyTTS.dialog);
		}
	}
}


//关闭api
void AronaGPTsoVITSProcess::GPTsoVITSapiClose()
{
	//生成请求
	//qDebug() << "00000000000000000000000000000000000001111111111111111111111111111111000000000000000000000000000000001111111111111111111111111111111";

	QUrl GPTsoVITSUrlclose(GPTsoVITSurl+"/control");
	QNetworkRequest aronaGPTsoVITSTTSrequest(GPTsoVITSUrlclose);
	aronaGPTsoVITSTTSrequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QJsonObject payload;
	payload["command"] = "exit";

	QByteArray jsonData = QJsonDocument(payload).toJson();

	//qDebug().noquote() << "POST URL:" << GPTsoVITSUrlclose.toString();
	//qDebug().noquote() << "POST Data:" << QString::fromUtf8(jsonData);

	QNetworkReply* networkReply = aronaGPTsoVITSNetworkManage->post(aronaGPTsoVITSTTSrequest, jsonData);
}