#include "addressbar.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QStyleOptionFrameV2>

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
    setButtonVisible(Right, true);
    setButtonPixmap(Right, QIcon(":/widgets/icons/refresh.png").pixmap(16));

    connect(this, SIGNAL(rightButtonClicked()), SLOT(onRightButtonClicked()));
}

void AddressBar::setLoading(bool yes)
{
    static QIcon iconCancel = QIcon(":/widgets/icons/cancelLoading.png");
    static QIcon iconRefresh = QIcon(":/widgets/icons/refresh.png");

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

QLinearGradient AddressBar::generateGradient(const QColor &color) const
{
    QColor defaultBaseColor = QApplication::palette().color(QPalette::Base);

    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, defaultBaseColor);
    gradient.setColorAt(0.15, color.lighter(120));
    gradient.setColorAt(0.5, color);
    gradient.setColorAt(0.85, color.lighter(120));
    gradient.setColorAt(1, defaultBaseColor);
    return gradient;
}

void AddressBar::paintEvent(QPaintEvent *event)
{
    QPalette p = palette();

    QColor defaultBaseColor = QApplication::palette().color(QPalette::Base);
    if (m_url.scheme() == QLatin1String("https")
        && p.color(QPalette::Text).value() < 128) {
        QColor lightYellow(248, 248, 210);
        p.setBrush(QPalette::Base, generateGradient(lightYellow));
    } else {
        p.setBrush(QPalette::Base, defaultBaseColor);
    }
    setPalette(p);

    FancyLineEdit::paintEvent(event);

    if (!hasFocus() && m_progress) {
        QPainter painter(this);
        QStyleOptionFrameV2 panel;
        initStyleOption(&panel);
        QRect backgroundRect = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

        QColor loadingColor = QColor(116, 192, 250);
        painter.save();
        painter.setBrush(generateGradient(loadingColor));
        painter.setPen(Qt::transparent);
        int mid = backgroundRect.width() / 100 * m_progress;
        QRect progressRect(backgroundRect.x(), backgroundRect.y(), mid, backgroundRect.height());
        painter.drawRect(progressRect);
        painter.restore();

        painter.setPen(palette().text().color());
        QFontMetrics fm = fontMetrics();
        QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);

        static int horizontalMargin = 2;
        static int verticalMargin = 1;
        int vscroll = 0;

        Qt::Alignment va = QStyle::visualAlignment(layoutDirection(), QFlag(alignment()));
        switch (va & Qt::AlignVertical_Mask) {
         case Qt::AlignBottom:
             vscroll = r.y() + r.height() - fm.height() - verticalMargin;
             break;
         case Qt::AlignTop:
             vscroll = r.y() + verticalMargin;
             break;
         default:
             //center
             vscroll = r.y() + (r.height() - fm.height() + 1) / 2;
             break;
        }
        QRect lineRect(r.x() + horizontalMargin, vscroll, r.width() - 2*horizontalMargin, fm.height());

        int minLB = qMax(0, -fm.minLeftBearing());
//            int minRB = qMax(0, -fm.minRightBearing());
        lineRect.adjust(minLB, 0, 0, 0);

        QString elidedText = fm.elidedText(text(), Qt::ElideRight, lineRect.width());
        painter.drawText(lineRect, va, elidedText);
    }

}

void AddressBar::updateUrl()
{
    QUrl url = QUrl::fromUserInput(text());
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
