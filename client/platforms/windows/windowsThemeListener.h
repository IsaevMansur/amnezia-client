#ifndef WINDOWSTHEMELISTENER_H
#define WINDOWSTHEMELISTENER_H

#include <QAbstractNativeEventFilter>
#include <QObject>

class WindowsThemeListener : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit WindowsThemeListener(QObject *parent = nullptr);
    ~WindowsThemeListener() override;

    Qt::ColorScheme colorScheme() const;
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

signals:
    void colorSchemeChanged(Qt::ColorScheme colorScheme);

private:
    void refreshColorScheme();

    Qt::ColorScheme m_colorScheme = Qt::ColorScheme::Unknown;
};

#endif
