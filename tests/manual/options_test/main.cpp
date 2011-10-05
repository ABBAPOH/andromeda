#include <QtGui/QApplication>
#include <QStringList>
#include <QDebug>

#include <options.h>

using namespace ExtensionSystem;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QStringList args1;
    QStringList args2;
    QStringList args3;
    QStringList args4;
    QStringList args5;
    QStringList args6;
    QStringList args7;
    QStringList args8;
    QStringList args9;

    args1 << QString("andromeda --option1 string --option2 10 --longoption 1.11 oN abc def").split(' ');
    args2 << QString("andromeda --option1 string --option3 10").split(' ');
    args3 << QString("andromeda --option1 string wrong --option2 10").split(' ');
    args4 << QString("andromeda --option1 string --option2 blabla").split(' ');
    args5 << QString("andromeda --option1 --option2 10").split(' ');
    args6 << QString("andromeda --option1 string bla bla bla").split(' ');
    args7 << QString("andromeda --option1 string 11.1 Yes bla").split(' ');
    args8 << QString("andromeda -1 string -2 10 -l 1.11 oN abc def").split(' ');
    args9 << QString("andromeda --files file1 file2 file3 file111").split(' ');

    Options opts;
    opts.addOption('1', "option1", Options::String, "String option");
    opts.addOption('2', "option2", Options::Int, "Int option");
    Option opt('l', "longoption", "Long option");
    opt.addValue(Options::Double, "double");
    opt.addValue(Options::Bool, "bool");
    opt.addValue(Options::String, "strings");
    opt.setMultiple(true);
    opts.addOption(opt);
//    opts.setDefaultOption("longoption");

    Option optFiles("files", "FILES!!!1");
    optFiles.addValue(Options::String, QString());
    optFiles.setMultiple(true);
    optFiles.setSingle(true);
    opts.addOption(optFiles);

    qDebug() << "parse2" << opts.parse(args2); qDebug(); opts.clear();
    qDebug() << "parse3" << opts.parse(args3); qDebug(); opts.clear();
    qDebug() << "parse4" << opts.parse(args4); qDebug(); opts.clear();
    qDebug() << "parse5" << opts.parse(args5); qDebug(); opts.clear();
    qDebug() << "parse6" << opts.parse(args6); qDebug(); opts.clear();
    qDebug() << "parse7" << opts.parse(args7); qDebug(); opts.clear();
    qDebug() << "parse8" << opts.parse(args8); qDebug(); opts.clear();
    qDebug() << "parse9" << opts.parse(args9); qDebug(); opts.clear();
    qDebug() << "parse1" << opts.parse(args1); qDebug(); /*opts.clear();*/

    qDebug() << opts.values();

//    return app.exec();
}
