#include "EditorTitleBar/EditorTitleBar.hpp"

#include "EditorTitleBar/EditorTitleBarInternal.hpp"

#include <QEvent>
#include <QFile>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWindow>

namespace Mirage::EditorQt {

EditorTitleBar::EditorTitleBar(QWidget* parent)
    : QWidget(parent) {
    auto* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    logoArea_ = CreateLogoArea();
    menuBarBackground_ = CreateStripe("EditorTitleBarMenuBar", kMenuBarHeight);
    viewportTitleBarBackground_ = CreateStripe(
        "EditorTitleBarViewportBar",
        kViewportTitleBarHeight
    );

    BuildMenuBar();
    BuildViewportTitleBar();
    ApplyStyles();
    ConnectWindowControls();

    menuBarBackground_->installEventFilter(this);

    auto* rightColumnLayout = new QVBoxLayout();
    rightColumnLayout->setContentsMargins(0, 0, 0, 0);
    rightColumnLayout->setSpacing(0);
    rightColumnLayout->addWidget(menuBarBackground_);
    rightColumnLayout->addWidget(viewportTitleBarBackground_);

    rootLayout->addWidget(logoArea_);
    rootLayout->addLayout(rightColumnLayout, 1);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(kMenuBarHeight + kViewportTitleBarHeight);
}

bool EditorTitleBar::eventFilter(QObject* watched, QEvent* event) {
#ifdef Q_OS_WIN
    if (watched == menuBarBackground_ && event->type() == QEvent::MouseButtonDblClick) {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            if (auto* hostWindow = window(); hostWindow != nullptr) {
                hostWindow->isMaximized()
                    ? hostWindow->showNormal()
                    : hostWindow->showMaximized();
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched, event);
#else
    Q_UNUSED(watched);
    Q_UNUSED(event);
    if (watched == menuBarBackground_) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
                if (auto* hostWindow = window(); hostWindow != nullptr) {
                    if (auto* nativeWindow = hostWindow->windowHandle();
                        nativeWindow != nullptr) {
                        nativeWindow->startSystemMove();
                        return true;
                    }
                }
            }
        }
    }

    return QWidget::eventFilter(watched, event);
#endif
}

bool EditorTitleBar::IsDragZone(const QPoint& localPosition) const {
    if (menuBarBackground_ == nullptr || !menuBarBackground_->geometry().contains(localPosition)) {
        return false;
    }

    if (QWidget* child = childAt(localPosition); child != nullptr) {
        return qobject_cast<QPushButton*>(child) == nullptr;
    }

    return true;
}

void EditorTitleBar::ApplyStyles() {
    QFile styleSheetFile(QString::fromUtf8(kTitleBarStyleSheetPath));
    if (!styleSheetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    setStyleSheet(QString::fromUtf8(styleSheetFile.readAll()));
}

void EditorTitleBar::BuildMenuBar() {
    menuBarLayout_ = new QHBoxLayout(menuBarBackground_);
    menuBarLayout_->setContentsMargins(4, 0, 0, 0);
    menuBarLayout_->setSpacing(4);

    fileButton_ = CreateMenuButton("File", kMenuButtonFileWidth);
    editButton_ = CreateMenuButton("Edit", kMenuButtonEditWidth);
    windowButton_ = CreateMenuButton("Window", kMenuButtonWindowWidth);

    menuBarLayout_->addWidget(fileButton_);
    menuBarLayout_->addWidget(editButton_);
    menuBarLayout_->addWidget(windowButton_);
    menuBarLayout_->addStretch(1);

    minimizeButton_ = CreateWindowControlButton(
        QString::fromUtf8("\u2212"),
        "WindowControlButton",
        kWindowControlButtonWidth,
        kWindowControlGlyphFontSize
    );
    maximizeButton_ = CreateWindowControlButton(
        QString::fromUtf8("\u25A1"),
        "WindowControlButton",
        kWindowControlButtonWidth,
        kWindowControlGlyphFontSize
    );
    closeButton_ = CreateWindowControlButton(
        QString::fromUtf8("\u00D7"),
        "CloseWindowControlButton",
        kWindowControlButtonWidth,
        kCloseControlGlyphFontSize
    );

    menuBarLayout_->addWidget(minimizeButton_);
    menuBarLayout_->addWidget(maximizeButton_);
    menuBarLayout_->addWidget(closeButton_);
}

void EditorTitleBar::BuildViewportTitleBar() {
    auto* viewportLayout = new QHBoxLayout(viewportTitleBarBackground_);
    viewportLayout->setContentsMargins(0, 0, 0, 0);
    viewportLayout->setSpacing(0);
    viewportLayout->addStretch(1);
}

void EditorTitleBar::ConnectWindowControls() {
    connect(minimizeButton_, &QPushButton::clicked, this, [this]() {
        if (auto* hostWindow = window(); hostWindow != nullptr) {
            hostWindow->showMinimized();
        }
    });

    connect(maximizeButton_, &QPushButton::clicked, this, [this]() {
        if (auto* hostWindow = window(); hostWindow != nullptr) {
            hostWindow->isMaximized()
                ? hostWindow->showNormal()
                : hostWindow->showMaximized();
        }
    });

    connect(closeButton_, &QPushButton::clicked, this, [this]() {
        if (auto* hostWindow = window(); hostWindow != nullptr) {
            hostWindow->close();
        }
    });
}

QFrame* EditorTitleBar::CreateStripe(const char* objectName, int height) {
    auto* stripe = new QFrame(this);
    stripe->setObjectName(objectName);
    stripe->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    stripe->setFixedHeight(height);
    return stripe;
}

QWidget* EditorTitleBar::CreateLogoArea() {
    auto* logoArea = new QFrame(this);
    logoArea->setObjectName("EditorLogoArea");
    logoArea->setFixedSize(kLogoAreaSize, kLogoAreaSize);

    auto* logoLayout = new QVBoxLayout(logoArea);
    logoLayout->setContentsMargins(0, 0, 0, 0);
    logoLayout->setAlignment(Qt::AlignCenter);

    const QPixmap logoPixmap(QString::fromUtf8(kLogoResourcePath));
    logoLabel_ = CreateLogoLabel(logoPixmap);
    logoLayout->addWidget(logoLabel_);

    return logoArea;
}

QPushButton* EditorTitleBar::CreateMenuButton(const QString& text, int width) {
    auto* button = new QPushButton(text, menuBarBackground_);
    button->setObjectName("EditorMenuButton");
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(width, kMenuButtonHeight);
    button->setFlat(true);
    return button;
}

QPushButton* EditorTitleBar::CreateWindowControlButton(
    const QString& text,
    const QString& objectName,
    int width,
    int fontSize
) {
    auto* button = new QPushButton(text, menuBarBackground_);
    button->setObjectName(objectName);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedSize(width, kMenuBarHeight);
    button->setFlat(true);
    button->setStyleSheet(QString("font-size: %1px;").arg(fontSize));
    return button;
}

QLabel* EditorTitleBar::CreateLogoLabel(const QPixmap& logoPixmap) {
    auto* label = new QLabel(this);
    label->setFixedSize(kLogoIconSize, kLogoIconSize);
    label->setScaledContents(false);
    label->setPixmap(
        logoPixmap.scaled(
            kLogoIconSize,
            kLogoIconSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        )
    );
    label->setAlignment(Qt::AlignCenter);
    return label;
}

} // namespace Mirage::EditorQt
