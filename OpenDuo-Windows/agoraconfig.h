#ifndef CAGORACONFIG_H
#define CAGORACONFIG_H
#include <memory>
#include <QSettings>
#include <QObject>
class CAgoraConfig
{
public:
    CAgoraConfig();

    void setAppId(const QString &str);
    QString getAppId();
    void setCallingId(const QString &str);
    QString getCallingId();
    void setChannelName(const QString &str);
    QString getChannelName();
    void setEnableVideo(bool bEnable);
    bool getEnableVideo();
    void setEnableAudio(bool bEnable);
    bool getEnableAudio();


	void setEnableBeauty(bool bEnable);
	bool getEnableBeauty();

	void setLigtheningContrastLevel(int level);
	int getLigtheningContrastLevel();

	void setRedness(int level);
	int getRedness();

	void setLightenging(int lightening);
	int getLightenging();

	void setSmoothness(int smooth);
	int getSmoothness();

    void getVideoResolution(int& width, int& height);

    QString getAppToken();
    int getFPS();
    int getBitrate();
    bool isCustomFPS();
    bool isCustomBitrate();
    bool isCustomResolution();
private:
    std::shared_ptr<QSettings>  m_spConfig;
};
#endif // CAGORACONFIG_H
