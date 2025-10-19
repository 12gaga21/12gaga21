#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Kaspersky Home Shield Browser");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Kaspersky Lab");
    app.setOrganizationDomain("kaspersky.com");
    
    // Set application style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Create data directory
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    
    qDebug() << "Kaspersky Home Shield Browser starting...";
    qDebug() << "Data directory:" << dataDir;
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}