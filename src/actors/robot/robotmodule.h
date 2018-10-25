#ifndef ROBOTMODULE_H
#define ROBOTMODULE_H

#include "robotmodulebase.h"
#include "robotview.h"
class RoboPult;

namespace ActorRobot
{
class EditLine;
class RobotModule;

class RobotModule : public RobotModuleBase
{
	Q_OBJECT
public:
	RobotModule(ExtensionSystem::KPlugin *parent);
	static RobotModule *self;

	QSize minimumSize() const;

public slots:
	// Reset actor state before program starts
	void reset();
	void terminateEvaluation() {}
	void loadActorData(QIODevice *source);

	void setAnimationEnabled(bool enabled);
	static QList<ExtensionSystem::CommandLineParameter> acceptableCommandLineParameters();
	QString initialize(
		const QStringList &configurationParameters,
		const ExtensionSystem::CommandLine &runtimeParameters
	);

private:
	// Actor methods
	void runGoUp();
	void runGoDown();
	void runGoLeft();
	void runGoRight();
	void runDoPaint();
	bool runIsWallAtTop();
	bool runIsWallAtBottom();
	bool runIsWallAtLeft();
	bool runIsWallAtRight();
	bool runIsFreeAtTop();
	bool runIsFreeAtBottom();
	bool runIsFreeAtLeft();
	bool runIsFreeAtRight();
	bool runIsColor();
	bool runIsClear();
	qreal runRadiation();
	int runTemperature();
	void runFieldSize(int &rows, int &cols);
	bool runMark(const int row, const int col) ;
	bool runColored(const int row, const int col);
	void runRobotPos(int &row, int &col);
	int runCellTemp(const int row, const int col);
	qreal runCellRad(const int row, const int col);
	QChar runUpChar(const int row, const int col);
	QChar runDownChar(const int row, const int col);
	bool isSafeToQuit();

public:
	// GUI access methods
	QWidget *mainWidget() const;
	QWidget *pultWidget() const;
	static ExtensionSystem::SettingsPtr robotSettings();
public slots:
	void reloadSettings(ExtensionSystem::SettingsPtr settings, const QStringList &keys);
	void changeGlobalState(ExtensionSystem::GlobalState old, ExtensionSystem::GlobalState current);
	void loadEnv();
	void resetEnv();
	void save2png();
	void saveEnv();
	void editEnv();
	void newEnv();
	void createNewField();
	void createRescentMenu();
	void updateLastFiles(const QString newFile);
	void openRecent();
	void setWindowSize();
	void copyFromPult(QString log);
	void getTimer();

private:
	void createGui();
	void updateRobot();
	void createEmptyField(int rows, int cols);
	int LoadFromFile(QString p_FileName);
	int SaveToFile(QString p_FileName);
	QWidget *m_mainWidget;
	// class QDeclarativeView * m_pultWidget;
	RoboPult *m_pultWidget;
	RoboField *field;
	RoboField *startField;
	bool animation;
	RobotView *view;
	QString curDir, curPDir;
	bool pressed;
	bool inDock;
	QPushButton *btnOK1;
	QPushButton *btnCancel1;  //Кнопки диалога новая обстановка
	QSpinBox *eXSizeEdit;
	QSpinBox *eYSizeEdit;
	QWidget *NewWindow;
	QMenu *rescentMenu;
	void prepareNewWindow();
	int CurCellSize;
	bool DISPLAY;
	ExtensionSystem::SettingsPtr curSettings;
	ConsoleField *curConsoleField;
	QMutex mutex;
	ExtensionSystem::GlobalState currentState;
	QTimer *redrawTimer;

signals:
	void sendToPultLog(const QVariant&);

}; // RobotModule

} // ActorRobot

#endif // ROBOTMODULE_H
