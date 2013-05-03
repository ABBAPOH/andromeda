#ifndef STANDARDCOMMANDS_P_H
#define STANDARDCOMMANDS_P_H

#include "standardcommands.h"

#include <QtCore/QMutex>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

namespace GuiSystem {

class StandardCommandsPrivate
{
public:
    StandardCommandsPrivate();

    static StandardCommandsPrivate *instance();

    static Command *createCommand(StandardCommands::StandardCommand command);
    static void retranslateCommand(Command *c, StandardCommands::StandardCommand command);

    static QByteArray commandId(StandardCommands::StandardCommand command);
    static QString commandText(StandardCommands::StandardCommand command);
    static QString commandToolTip(StandardCommands::StandardCommand command);
    static QKeySequence commandShortcut(StandardCommands::StandardCommand command);
    static QIcon commandIcon(StandardCommands::StandardCommand command);

    QMutex mutex;
    Command *commands[StandardCommands::StandardCommandCount];
};

} // namespace GuiSystem

#endif // STANDARDCOMMANDS_P_H
