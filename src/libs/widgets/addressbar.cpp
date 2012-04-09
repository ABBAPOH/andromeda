#include "addressbar.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtGui/QApplication>
#include <QtGui/QPaintEvent>

static inline QString urlToUserOutput(const QUrl &url)
{
    if (url.scheme() == QLatin1String("file"))
        return url.toLocalFile();
    else if (url.scheme() == qApp->applicationName())
        return QString();
    else
        return url.toString();
}

AddressBar::AddressBar(QWidget *parent) :
    FancyLineEdit(parent),
    m_loading(false),
    m_progress(0)
{
    setFrame(false);
    setButtonVisible(Right, true);
    setButtonPixmap(Right, QIcon(":/icons/refresh.png").pixmap(16));

    connect(this, SIGNAL(rightButtonClicked()), SLOT(onRightButtonClicked()));
}

void AddressBar::setLoading(bool yes)
{
    static QIcon iconCancel = QIcon(":/icons/cancelLoading.png");
    static QIcon iconRefresh = QIcon(":/icons/refresh.png");

    m_loading = yes;
    if (yes)
        setButtonPixmap(Right, iconCancel.pixmap(16));
    else
        setButtonPixmap(Right, iconRefresh.pixmap(16));
}

void AddressBar::setUrl(const QUrl &url)
{
    m_url = url;
    setText(urlToUserOutput(url));
}

void AddressBar::startLoad()
{
    setLoading(true);
}

void AddressBar::finishLoad()
{
    setLoading(false);
    m_progress = 0;
    update();
}

void AddressBar::setLoadProgress(int progress)
{
    if (!m_loading)
        setLoading(true);
    m_progress = progress;
    update();
}

void AddressBar::keyPressEvent(QKeyEvent *e)
{
    QLineEdit::keyPressEvent(e);

    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        updateUrl();
    }
}

void AddressBar::paintEvent(QPaintEvent *event)
{
    QPalette p = palette();

    QColor defaultBaseColor = QApplication::palette().color(QPalette::Base);
    QColor backgroundColor = defaultBaseColor;
    if (m_url.scheme() == QLatin1String("https")
        && p.color(QPalette::Text).value() < 128) {
        QColor lightYellow(248, 248, 210);
        backgroundColor = lightYellow;
    }

    // set the progress bar
    if (m_progress == 0) {
        p.setBrush(QPalette::Base, backgroundColor);
    } else {
        QColor loadingColor = QColor(116, 192, 250);
        if (p.color(QPalette::Text).value() >= 128)
            loadingColor = defaultBaseColor.darker(200);

        QLinearGradient gradient(0, 0, width(), 0);
        gradient.setColorAt(0, loadingColor);
        gradient.setColorAt(m_progress/100.0, backgroundColor);
        p.setBrush(QPalette::Base, gradient);
    }
    setPalette(p);

    FancyLineEdit::paintEvent(event);
}

void AddressBar::updateUrl()
{
    QUrl url;
    if (m_url.scheme() == QLatin1String("file")) {
        // we try to resolve local paths
        QFileInfo info(text());
        if (info.isAbsolute()) {
            if (info.exists())
                url = QUrl::fromLocalFile(info.canonicalFilePath());
        } else {
            QDir dir(m_url.toLocalFile());
            QString path = QDir::cleanPath(dir.absoluteFilePath(text()));
            QFileInfo info(path);
            if (info.exists())
                url = QUrl::fromLocalFile(path);
        }
    } else {
        url = QUrl::fromUserInput(text());
    }

    if (m_url != url) {
        setText(urlToUserOutput(url));
        m_url = url;
        emit open(m_url);
    } else {
        emit refresh();
    }
}

void AddressBar::onRightButtonClicked()
{
    if (m_loading)
        emit canceled();
    else
        updateUrl();
}
