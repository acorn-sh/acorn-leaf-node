#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QString>
#include <QStringList>
#include <QIcon>
#include <QProcessEnvironment>
#include <QCoreApplication>
#include <QDir>

class ResourceManager {
public:
    ResourceManager();

    QString getPythonPath() const;
    QString getScriptPath(const QString& scriptName) const;
    QString getFilePath(const QString& fileName) const;
    QStringList getImagePaths() const;
    QIcon getIcon(const QString& iconName) const;
    QStringList getLogoPaths() const;
    QProcessEnvironment getProcessEnvironment() const;

private:
    QString appDirPath;
    QString pythonPath;
    QProcessEnvironment env;

    void setupEnvironment();
};

#endif // RESOURCEMANAGER_H
