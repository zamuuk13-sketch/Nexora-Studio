#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    buildUi(); applyStyle();
    connect(&m_bridge, &BridgeServer::studioChanged, this, &MainWindow::bridgeState);
    m_bridge.start();
}

void MainWindow::buildUi() {
    resize(1180,760); setWindowTitle("Nexora");
    auto *central=new QWidget(this); auto *layout=new QHBoxLayout(central); layout->setContentsMargins(0,0,0,0);
    auto *sidebar=new QWidget; sidebar->setFixedWidth(250); auto *side=new QVBoxLayout(sidebar); side->setContentsMargins(18,20,18,20);
    auto *brand=new QLabel("NEXORA"); brand->setObjectName("Brand"); side->addWidget(brand); side->addSpacing(24); side->addWidget(new QLabel("PROJECT"));
    auto *projects=new QListWidget; projects->addItem("My Roblox Project"); side->addWidget(projects); side->addStretch();
    m_status=new QLabel("○  Studio disconnected"); side->addWidget(m_status); layout->addWidget(sidebar);
    auto *main=new QWidget; auto *ml=new QVBoxLayout(main); ml->setContentsMargins(28,22,28,22);
    auto *header=new QHBoxLayout; auto *title=new QLabel("AI Workspace"); title->setObjectName("Title"); header->addWidget(title); header->addStretch();
    m_aiSelector=new QComboBox; m_aiSelector->addItem("Add an AI..."); header->addWidget(m_aiSelector);
    auto *add=new QPushButton("+  AI"); connect(add,&QPushButton::clicked,this,&MainWindow::addAI); header->addWidget(add); ml->addLayout(header);
    m_chat=new QPlainTextEdit; m_chat->setReadOnly(true); m_chat->setPlaceholderText("Nexora activity will appear here..."); ml->addWidget(m_chat,1);
    auto *composer=new QHBoxLayout; m_prompt=new QPlainTextEdit; m_prompt->setFixedHeight(100); m_prompt->setPlaceholderText("Tell Nexora what to build in Roblox Studio..."); composer->addWidget(m_prompt,1);
    auto *send=new QPushButton("Build  →"); send->setFixedWidth(120); connect(send,&QPushButton::clicked,this,&MainWindow::sendPrompt); composer->addWidget(send); ml->addLayout(composer);
    layout->addWidget(main,1); setCentralWidget(central);
}

void MainWindow::applyStyle(){setStyleSheet(R"(QWidget{background:#0b0b0d;color:#f4f4f5;font-family:'Segoe UI';}#Brand{font-size:22px;font-weight:700;letter-spacing:3px;}#Title{font-size:20px;font-weight:600;}QListWidget,QPlainTextEdit,QComboBox{background:#111114;border:1px solid #242429;border-radius:10px;padding:8px;}QPushButton{background:#f4f4f5;color:#0b0b0d;border:0;border-radius:9px;padding:10px 14px;font-weight:600;}QPushButton:hover{background:#dcdce1;})");}

void MainWindow::sendPrompt(){
    const QString prompt=m_prompt->toPlainText().trimmed(); if(prompt.isEmpty())return; m_chat->appendPlainText("You  ·  "+prompt);
    if(!m_bridge.running()){m_chat->appendPlainText("Nexora  ·  Bridge could not start on localhost:28473.");return;}
    const auto plan=m_agent.planLocalTools(prompt);
    for(const auto &item:plan){const auto o=item.toObject();m_bridge.enqueue(o.value("command").toString(),o.value("args").toObject());m_chat->appendPlainText("Nexora  ·  queued "+o.value("command").toString());}
    m_prompt->clear();
}

void MainWindow::addAI(){
    bool ok=false; const QString name=QInputDialog::getText(this,"Add AI","AI name:",QLineEdit::Normal,"My AI",&ok); if(!ok||name.trimmed().isEmpty())return;
    AIProfile p; p.name=name.trimmed(); p.provider="OpenAI-compatible"; p.model="default"; p.endpoint="https://api.openai.com/v1/chat/completions"; m_profiles.append(p);
    m_aiSelector->insertItem(m_aiSelector->count()-1,p.name);m_aiSelector->setCurrentText(p.name);m_agent.setProfile(p);
}
void MainWindow::bridgeState(bool connected){m_status->setText(connected?"●  Studio connected":"○  Studio disconnected");}
