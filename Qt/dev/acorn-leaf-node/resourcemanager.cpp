#include "ResourceManager.h"

ResourceManager::ResourceManager()
    : appDirPath(QCoreApplication::applicationDirPath())
{
#ifdef Q_OS_MACOS
    pythonPath = appDirPath + "/bundled_python/bin/python3.11";
#elif defined(Q_OS_WIN)
    pythonPath = appDirPath + "\\bundled_python\\Scripts\\python.exe";
#elif defined(Q_OS_LINUX)
    pythonPath = appDirPath + "/bundled_python/bin/python3.11";
#else
    pythonPath = "python3";  // Fallback to system python if platform not detected
#endif

    // Set up the environment variables for process execution
    env = QProcessEnvironment::systemEnvironment();

#ifdef Q_OS_MACOS
    QString path = env.value("PATH");
    if (!path.contains("/usr/local/bin")) {
        path = "/usr/local/bin:" + path;
        env.insert("PATH", path);
    }
#elif defined(Q_OS_WIN)
    QString path = env.value("PATH");
    if (!path.contains(appDirPath + "\\bundled_python\\Scripts")) {
        path = appDirPath + "\\bundled_python\\Scripts;" + path;
        env.insert("PATH", path);
    }
#elif defined(Q_OS_LINUX)
    QString path = env.value("PATH");
    if (!path.contains("/usr/local/bin")) {
        path = "/usr/local/bin:" + path;
        env.insert("PATH", path);
    }
#endif
}

QString ResourceManager::getPythonPath() const {
    return pythonPath;
}

QString ResourceManager::getScriptPath(const QString& scriptName) const {
    return appDirPath + QDir::separator() + scriptName;
}

QString ResourceManager::getFilePath(const QString& fileName) const {
    return appDirPath + QDir::separator() + fileName;
}

QStringList ResourceManager::getImagePaths() const {
    return QStringList({":/images/feature1.png", ":/images/feature2.png", ":/images/feature3.png"});
}

QIcon ResourceManager::getIcon(const QString& iconName) const {
    return QIcon(":/images/icons/" + iconName + ".svg");
}

QProcessEnvironment ResourceManager::getProcessEnvironment() const {
    return env;
}
