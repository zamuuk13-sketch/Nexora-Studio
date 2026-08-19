#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QLabel>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include "bridge/BridgeServer.h"
#include "agent.h"

class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
private slots:
    void sendPrompt();
    void addAI();
    void bridgeState(bool connected);
    void showHome();
    void showWorkspace();
    void createProject();
    void configureFirstAI();
private:
    void buildUi();
    void buildHome();
    void buildWorkspace();
    void applyStyle();
    QList<AIProfile> m_profiles;
    BridgeServer m_bridge;
    Agent m_agent;
    QStackedWidget *m_pages=nullptr;
    QWidget *m_home=nullptr;
    QWidget *m_workspace=nullptr;
    QComboBox *m_aiSelector=nullptr;
    QPlainTextEdit *m_chat=nullptr;
    QPlainTextEdit *m_prompt=nullptr;
    QLabel *m_status=nullptr;
    QLabel *m_projectTitle=nullptr;
    QLineEdit *m_projectName=nullptr;
};
