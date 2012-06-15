#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QActionGroup;
class QImageView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static QList<MainWindow*> windows();

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

public slots:
    void about();

    void open();
    void open(const QString &file);
    static void openWindow(const QString &file);
    static void openWindow(const QStringList &files);
    static void newWindow();
    void save();
    void saveAs();

    static void preferences();

    void resizeImage();
    void easterEgg();

    void updateSaveActions();

protected:
    void closeEvent(QCloseEvent *);
    void hideEvent(QHideEvent *);
    void showEvent(QShowEvent *);
    void changeEvent(QEvent *);
    void resizeEvent(QResizeEvent *);

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupFileMenu();
    void setupEditMenu();
    void setupViewMenu();
    void setupToolsMenu();
    void setupHelpMenu();
    void setupConnections();
    void retranslateUi();
    void updateTitle();

private:
    QImageView *view;
    QString m_file;

    QMenuBar *m_menuBar;
    QToolBar *m_toolBar;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;

    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSaveAs;
    QAction *actionClose;
    QAction *actionQuit;

    QAction *actionPreferences;
    QAction *actionResize;

    QAction *actionAbout;
    QAction *actionAboutQt;
};

#endif // MAINWINDOW_H
