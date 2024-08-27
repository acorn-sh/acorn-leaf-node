#include "ResourceManager.h"

ResourceManager::ResourceManager()
    : appDirPath(QCoreApplication::applicationDirPath())
{
    // Setup Python path based on the operating system
#ifdef Q_OS_MACOS
    pythonPath = appDirPath + "/bundled_python/bin/python3.11";
#elif defined(Q_OS_WIN)
    pythonPath = appDirPath + "\\bundled_python\\Scripts\\python.exe";
#elif defined(Q_OS_LINUX)
    pythonPath = appDirPath + "/bundled_python/bin/python3.11";
#else
    pythonPath = "python3";  // Fallback to system python if platform not detected
#endif

    setupEnvironment();
}

void ResourceManager::setupEnvironment() {
    // Initialize the process environment
    env = QProcessEnvironment::systemEnvironment();

    // Setup PATH environment variable based on the operating system
    QString path = env.value("PATH");

#ifdef Q_OS_MACOS
    QString dockerPath = "/usr/local/bin";  // Common path for Docker on macOS
    if (!path.contains(dockerPath)) {
        path = dockerPath + ":" + path;
        env.insert("PATH", path);
    }
#elif defined(Q_OS_WIN)
    QString dockerPath = "C:\\Program Files\\Docker\\Docker\\resources\\bin";
    if (!path.contains(dockerPath)) {
        path = dockerPath + ";" + path;
        env.insert("PATH", path);
    }
#elif defined(Q_OS_LINUX)
    QString dockerPath = "/usr/local/bin";  // Common path for Docker on Linux
    if (!path.contains(dockerPath)) {
        path = dockerPath + ":" + path;
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

QProcessEnvironment ResourceManager::getProcessEnvironment() const {
    return env;
}

QString ResourceManager::getFilePath(const QString& fileName) const {
    return appDirPath + QDir::separator() + fileName;
}

QStringList ResourceManager::getImagePaths() const {
    return QStringList({":/images/feature1.png", ":/images/feature2.png", ":/images/feature3.png"});
}
QStringList ResourceManager::getLogoPaths() const {
    return QStringList({":/images/acorn_sh_logo.png"});
}

QIcon ResourceManager::getIcon(const QString& iconName) const {
    return QIcon(":/images/icons/" + iconName + ".svg");
}
