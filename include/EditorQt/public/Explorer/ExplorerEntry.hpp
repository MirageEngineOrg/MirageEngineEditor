#pragma once

#include <QObject>

namespace Mirage::EditorQt {

class ExplorerFolderEntry;

class ExplorerEntry : public QObject {
    Q_OBJECT

public:
    enum class Kind {
        File,
        Folder
    };

    explicit ExplorerEntry(QString name, Kind kind, ExplorerFolderEntry* parentFolder = nullptr);
    ~ExplorerEntry() override = default;

    [[nodiscard]] Kind GetKind() const noexcept;
    [[nodiscard]] const QString& GetName() const noexcept;
    [[nodiscard]] ExplorerFolderEntry* GetParentFolder() const noexcept;

signals:
    void ContentChanged();

private:
    QString name_;
    Kind kind_ {Kind::File};
    ExplorerFolderEntry* parentFolder_ {nullptr};
};

} // namespace Mirage::EditorQt
