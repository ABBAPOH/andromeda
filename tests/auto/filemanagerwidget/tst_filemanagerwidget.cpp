#include <QtCore/QString>
#include <QtTest/QtTest>

#include <FileManager/FileManagerWidget>

using namespace FileManager;

class FileManagerWidgetTest : public QObject
{
    Q_OBJECT

public:
    FileManagerWidgetTest();

private Q_SLOTS:
    void testDefaultValues();
    void testState();
};

FileManagerWidgetTest::FileManagerWidgetTest()
{
}

void FileManagerWidgetTest::testDefaultValues()
{
    FileManagerWidget widget;
    QCOMPARE(widget.alternatingRowColors(), true);
    QCOMPARE(widget.currentPath(), QString());
    QCOMPARE(widget.flow(), FileManagerWidget::LeftToRight);
#ifdef Q_OS_MAC
    QCOMPARE(widget.gridSize(), QSize(128, 128));
    QCOMPARE(widget.iconSize(FileManagerWidget::IconView), QSize(64, 64));
#else
    QCOMPARE(widget.gridSize(), QSize(96, 96));
    QCOMPARE(widget.iconSize(FileManagerWidget::IconView), QSize(32, 32));
#endif
    QCOMPARE(widget.iconSize(FileManagerWidget::ColumnView), QSize(16, 16));
    QCOMPARE(widget.iconSize(FileManagerWidget::TreeView), QSize(16, 16));
    QCOMPARE(widget.itemsExpandable(), true);
    QCOMPARE(widget.selectedPaths(), QStringList());
    QCOMPARE(widget.showHiddenFiles(), false);
    QCOMPARE(widget.sortingColumn(), FileManagerWidget::NameColumn);
    QCOMPARE(widget.sortingOrder(), Qt::AscendingOrder);
    QCOMPARE(widget.viewMode(), FileManagerWidget::IconView);
}

void FileManagerWidgetTest::testState()
{
    FileManagerWidget widget;
    widget.setAlternatingRowColors(false);
    widget.setCurrentPath(QDir::homePath());
    widget.setFlow(FileManagerWidget::TopToBottom);
    widget.setGridSize(QSize(64, 64));
    widget.setIconSize(FileManagerWidget::IconView, QSize(16, 16));
    widget.setIconSize(FileManagerWidget::ColumnView, QSize(32, 32));
    widget.setIconSize(FileManagerWidget::TreeView, QSize(32, 32));
    widget.setItemsExpandable(false);
    widget.setShowHiddenFiles(true);
    widget.setSortingColumn(FileManagerWidget::DateColumn);
    widget.setSortingOrder(Qt::DescendingOrder);
    widget.setViewMode(FileManagerWidget::ColumnView);

    QByteArray state = widget.saveState();
    FileManagerWidget widget2;
    widget2.restoreState(state);

    QCOMPARE(widget2.alternatingRowColors(), widget.alternatingRowColors());
    QCOMPARE(widget2.currentPath(), widget.currentPath());
    QCOMPARE(widget2.flow(), widget.flow());
    QCOMPARE(widget2.gridSize(), widget.gridSize());
    QCOMPARE(widget2.iconSize(FileManagerWidget::IconView), widget.iconSize(FileManagerWidget::IconView));
    QCOMPARE(widget2.iconSize(FileManagerWidget::ColumnView), widget.iconSize(FileManagerWidget::ColumnView));
    QCOMPARE(widget2.iconSize(FileManagerWidget::TreeView), widget.iconSize(FileManagerWidget::TreeView));
    QCOMPARE(widget2.itemsExpandable(), widget.itemsExpandable());
    QCOMPARE(widget2.showHiddenFiles(), widget.showHiddenFiles());
    QCOMPARE(widget2.sortingColumn(), widget.sortingColumn());
    QCOMPARE(widget2.sortingOrder(), widget.sortingOrder());
    QCOMPARE(widget2.viewMode(), widget.viewMode());

    // test empty state
    state.clear();
    QVERIFY(!widget2.restoreState(state));
    // test no data
    state = "F15t\1";
    QVERIFY(!widget2.restoreState(state));
    // test invalid version
    state = "F15t\2";
    QVERIFY(!widget2.restoreState(state));
    // test invalid magic
    state = "F15r";
    QVERIFY(!widget2.restoreState(state));
}

QTEST_MAIN(FileManagerWidgetTest)

#include "tst_filemanagerwidget.moc"
