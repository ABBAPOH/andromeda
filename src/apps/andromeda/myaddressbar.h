#ifndef MYADDRESSBAR_H
#define MYADDRESSBAR_H

#include <Widgets/AddressBar>

namespace Andromeda {

class MyAddressBar : public AddressBar
{
    Q_OBJECT
    Q_DISABLE_COPY(MyAddressBar)
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

private:
    QAction *actions[ActionCount];
};

} // namespace Andromeda

#endif // MYADDRESSBAR_H
