#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include <ExtensionSystem/MutablePluginSpec>
#include <ExtensionSystem/PluginEditor>

class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void open();
    void open(const QString &);
    void save();
    void save(const QString &);
    void saveAs();

private:
    void setupUi();
    ExtensionSystem::PluginSpec::Format format();

private:
    QMenu *fileMenu;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *quitAction;

    QMenu *formatMenu;
    QAction *saveXmlAction;
    QAction *saveBinaryAction;

    ExtensionSystem::MutablePluginSpec *pluginSpec;
    ExtensionSystem::PluginEditor *m_editor;
    QSettings *settings;
    QString currentFile;
};

#endif // MAINWINDOW_H
