#pragma once
#include <QMainWindow>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QLabel>
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
private:
    void buildUi();
    void applyStyle();
    QList<AIProfile> m_profiles;
    BridgeServer m_bridge;
    Agent m_agent;
    QComboBox *m_aiSelector = nullptr;
    QPlainTextEdit *m_chat = nullptr;
    QPlainTextEdit *m_prompt = nullptr;
    QLabel *m_status = nullptr;
};
