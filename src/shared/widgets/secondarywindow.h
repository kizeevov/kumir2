#ifndef SECONDARY_WINDOW_H
#define SECONDARY_WINDOW_H

#include "extensionsystem/settings.h"

#include <QtCore>
#include <QtGui>

#ifdef WIDGETS_LIBRARY
#define WIDGETS_EXPORT Q_DECL_EXPORT
#else
#define WIDGETS_EXPORT Q_DECL_IMPORT
#endif

namespace Widgets {


class WIDGETS_EXPORT SecondaryWindow :
        public QWidget
{
    friend class SecondaryWindowImpl;
    Q_OBJECT
    Q_PROPERTY(QString windowTitle READ windowTitle WRITE setWindowTitle)
    Q_PROPERTY(bool stayOnTop READ isStayOnTop WRITE setStayOnTop)
public:
    explicit SecondaryWindow(QWidget *centralComponent,
                             QMainWindow * mainWindow,
                             ExtensionSystem::SettingsPtr settings,
                             const QString &settingsKey);

    explicit SecondaryWindow(QWidget *centralComponent,
                             class DockWindowPlace * dockPlace,
                             QMainWindow * mainWindow,
                             ExtensionSystem::SettingsPtr settings,
                             const QString & settingsKey
                             );
    ~SecondaryWindow();
public slots:
    void setVisible(bool visible);
    void setWindowTitle(const QString &);
    void close();
    void showMinimized();
    bool isFloating() const;
    void toggleDocked();
    QString windowTitle();
    void setStayOnTop(bool v);
    bool isStayOnTop();
    QAction * toggleViewAction() const;
    void setSettingsObject(ExtensionSystem::SettingsPtr settings);
    void restoreState();
    void saveState();
    void paintEvent(QPaintEvent *e);
public:
    QSize minimumSizeHint() const;
protected:
    void closeEvent(QCloseEvent *);
protected slots:
    void activate();
private:
    QScopedPointer<class SecondaryWindowImpl> pImpl_;
};


}

#endif // CUSTOMWINDOW_H
