#ifndef MYADDRESSBAR_H
#define MYADDRESSBAR_H

#include <widgets/addressbar.h>

namespace Andromeda {

class MyAddressBar : public AddressBar
{
    Q_OBJECT

public:
    enum Action { NoAction = -1,
                Redo,
                Undo,
                Cut,
                Copy,
                Paste,
                SelectAll,
                ActionCount
                };

    explicit MyAddressBar(QWidget *parent = 0);

private:
    void createActions();
    void retranslateUi();
    void registerActions();

private:
    QAction *actions[ActionCount];
};

} // namespace Andromeda

#endif // MYADDRESSBAR_H
