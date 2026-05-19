#pragma once

#include <QWidget>

class QString;

namespace Mirage::EditorQt {

class DockingWidget : public QWidget {
public:
    explicit DockingWidget(QString title, bool closable, QWidget* parent = nullptr);
    ~DockingWidget() override = default;

    [[nodiscard]] bool IsClosable() const noexcept;
    [[nodiscard]] const QString& GetTitle() const noexcept;

private:
    QString title_;
    bool closable_;
};

} // namespace Mirage::EditorQt
