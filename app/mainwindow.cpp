#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QFrame>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>

static QFrame *card() { auto *f=new QFrame; f->setObjectName("Card"); return f; }
static QLabel *muted(const QString &s) { auto *l=new QLabel(s); l->setObjectName("Muted"); return l; }

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    buildUi(); applyStyle();
    connect(&m_bridge,&BridgeServer::studioChanged,this,&MainWindow::bridgeState);
    m_bridge.start();
}

void MainWindow::buildUi() {
    resize(1280,820); setMinimumSize(1050,700); setWindowTitle("Nexora");
    m_pages=new QStackedWidget(this); setCentralWidget(m_pages);
    buildHome(); buildWorkspace();
    showHome();
}

void MainWindow::buildHome() {
    m_home=new QWidget; auto *root=new QVBoxLayout(m_home); root->setContentsMargins(70,55,70,55); root->setSpacing(22);
    auto *top=new QHBoxLayout; auto *brand=new QLabel("NEXORA"); brand->setObjectName("Brand"); top->addWidget(brand); top->addStretch(); auto *status=muted("AI-powered Roblox creation"); top->addWidget(status); root->addLayout(top);
    auto *hero=new QWidget; auto *hl=new QVBoxLayout(hero); hl->setContentsMargins(0,50,0,20);
    auto *h=new QLabel("Build worlds, not code."); h->setObjectName("Hero"); hl->addWidget(h);
    auto *sub=new QLabel("Describe what you want. Nexora plans, builds and verifies it directly in Roblox Studio."); sub->setObjectName("HeroSub"); sub->setWordWrap(true); hl->addWidget(sub); root->addWidget(hero);
    auto *grid=new QGridLayout; grid->setSpacing(16);
    auto *newCard=card(); auto *nl=new QVBoxLayout(newCard); nl->setContentsMargins(24,24,24,24); auto *nt=new QLabel("Create a project"); nt->setObjectName("CardTitle"); nl->addWidget(nt); nl->addWidget(muted("Start a fresh Roblox project with an AI workspace.")); nl->addStretch(); auto *nb=new QPushButton("New project  →"); connect(nb,&QPushButton::clicked,this,&MainWindow::createProject); nl->addWidget(nb); grid->addWidget(newCard,0,0);
    auto *aiCard=card(); auto *al=new QVBoxLayout(aiCard); al->setContentsMargins(24,24,24,24); auto *at=new QLabel("AI providers"); at->setObjectName("CardTitle"); al->addWidget(at); al->addWidget(muted("Connect Gemini, OpenAI-compatible APIs or your own endpoint.")); al->addStretch(); auto *ab=new QPushButton("Configure AI  →"); connect(ab,&QPushButton::clicked,this,&MainWindow::configureFirstAI); al->addWidget(ab); grid->addWidget(aiCard,0,1);
    root->addLayout(grid); root->addStretch(); auto *foot=muted("Nexora Desktop  ·  Local Studio bridge  ·  v1.0"); root->addWidget(foot);
    m_pages->addWidget(m_home);
}

void MainWindow::buildWorkspace() {
    m_workspace=new QWidget; auto *outer=new QHBoxLayout(m_workspace); outer->setContentsMargins(0,0,0,0); outer->setSpacing(0);
    auto *side=new QWidget; side->setObjectName("Sidebar"); side->setFixedWidth(245); auto *sl=new QVBoxLayout(side); sl->setContentsMargins(16,20,16,18); sl->setSpacing(8);
    auto *brand=new QLabel("NEXORA"); brand->setObjectName("BrandSmall"); sl->addWidget(brand); sl->addSpacing(18);
    auto *home=new QPushButton("⌂   Home"); connect(home,&QPushButton::clicked,this,&MainWindow::showHome); sl->addWidget(home);
    sl->addWidget(muted("PROJECT")); auto *tree=new QListWidget; tree->addItem("▾  Roblox Project"); tree->addItem("    Workspace"); tree->addItem("    ServerScriptService"); tree->addItem("    ReplicatedStorage"); sl->addWidget(tree,1);
    sl->addWidget(muted("TOOLS")); auto *tools=new QListWidget; tools->addItems({"▣  Maps","♙  NPCs","✦  Animations","◈  UI","⚔  Combat","⌁  Scripts"}); sl->addWidget(tools,1);
    m_status=new QLabel("○  Studio disconnected"); m_status->setObjectName("Status"); sl->addWidget(m_status); outer->addWidget(side);
    auto *content=new QWidget; auto *cl=new QVBoxLayout(content); cl->setContentsMargins(28,22,28,22); cl->setSpacing(16);
    auto *header=new QHBoxLayout; m_projectTitle=new QLabel("Workspace"); m_projectTitle->setObjectName("Title"); header->addWidget(m_projectTitle); header->addStretch();
    m_aiSelector=new QComboBox; m_aiSelector->setMinimumWidth(190); m_aiSelector->addItem("Configure an AI..."); header->addWidget(m_aiSelector); auto *add=new QPushButton("+  Add AI"); connect(add,&QPushButton::clicked,this,&MainWindow::addAI); header->addWidget(add); cl->addLayout(header);
    m_chat=new QPlainTextEdit; m_chat->setReadOnly(true); m_chat->setPlaceholderText("Nexora's build activity will appear here..."); cl->addWidget(m_chat,1);
    auto *composer=card(); auto *co=new QVBoxLayout(composer); co->setContentsMargins(14,14,14,14); auto *label=muted("NEXORA  ·  BUILD COMMAND"); co->addWidget(label);
    auto *row=new QHBoxLayout; m_prompt=new QPlainTextEdit; m_prompt->setFixedHeight(92); m_prompt->setPlaceholderText("Describe what you want Nexora to create in Roblox Studio..."); row->addWidget(m_prompt,1); auto *send=new QPushButton("Build  →"); send->setObjectName("BuildButton"); send->setFixedWidth(125); connect(send,&QPushButton::clicked,this,&MainWindow::sendPrompt); row->addWidget(send); co->addLayout(row); cl->addWidget(composer);
    outer->addWidget(content,1); m_pages->addWidget(m_workspace);
}

void MainWindow::applyStyle() {
    setStyleSheet(R"(
QWidget{background:#09090b;color:#f5f5f7;font-family:"Segoe UI";font-size:13px;}#Sidebar{background:#0d0d10;border-right:1px solid #202024;}#Brand{font-size:30px;font-weight:800;letter-spacing:7px;}#BrandSmall{font-size:19px;font-weight:800;letter-spacing:4px;}#Hero{font-size:48px;font-weight:700;}#HeroSub{font-size:17px;color:#9b9ba3;max-width:760px;}#Title{font-size:22px;font-weight:650;}#Card{background:#111115;border:1px solid #24242a;border-radius:16px;}#CardTitle{font-size:18px;font-weight:650;}#Muted{color:#85858e;}#Status{padding:9px;color:#8f8f98;}QListWidget,QPlainTextEdit,QComboBox{background:#101014;border:1px solid #25252b;border-radius:11px;padding:8px;}QListWidget::item{padding:8px;border-radius:7px;}QListWidget::item:selected{background:#202026;}QComboBox{padding:10px 12px;}QPushButton{background:#19191e;color:#f5f5f7;border:1px solid #29292f;border-radius:10px;padding:10px 14px;font-weight:600;}QPushButton:hover{background:#24242a;}#BuildButton{background:#f4f4f5;color:#09090b;border:0;font-size:14px;}#BuildButton:hover{background:#dcdce2;})");
}

void MainWindow::showHome(){m_pages->setCurrentWidget(m_home);}
void MainWindow::showWorkspace(){m_pages->setCurrentWidget(m_workspace);}

void MainWindow::createProject(){
    bool ok=false; const QString name=QInputDialog::getText(this,"New project","Project name:",QLineEdit::Normal,"My Roblox World",&ok); if(!ok||name.trimmed().isEmpty())return;
    m_projectTitle->setText(name.trimmed()); showWorkspace();
    if(!hasAI()) configureFirstAI();
}

void MainWindow::configureFirstAI(){
    QDialog d(this); d.setWindowTitle("Configure AI"); d.setMinimumWidth(480); auto *l=new QVBoxLayout(&d); l->addWidget(new QLabel("Connect an AI to power Nexora."));
    auto *name=new QLineEdit; name->setPlaceholderText("AI name (e.g. Gemini Pro)"); l->addWidget(name);
    auto *provider=new QComboBox; provider->addItems({"Gemini","OpenAI-compatible","Custom API"}); l->addWidget(provider);
    auto *model=new QLineEdit; model->setPlaceholderText("Model (e.g. gemini-2.5-flash)"); l->addWidget(model);
    auto *key=new QLineEdit; key->setPlaceholderText("API key"); key->setEchoMode(QLineEdit::Password); l->addWidget(key);
    auto *endpoint=new QLineEdit; endpoint->setPlaceholderText("Endpoint (optional for Gemini)"); l->addWidget(endpoint);
    auto *prompt=new QPlainTextEdit; prompt->setPlaceholderText("Optional system instructions for this AI..."); prompt->setFixedHeight(80); l->addWidget(prompt);
    auto *buttons=new QDialogButtonBox(QDialogButtonBox::Save|QDialogButtonBox::Cancel); l->addWidget(buttons); connect(buttons,&QDialogButtonBox::accepted,&d,&QDialog::accept); connect(buttons,&QDialogButtonBox::rejected,&d,&QDialog::reject);
    if(d.exec()!=QDialog::Accepted)return;
    AIProfile p; p.name=name->text().trimmed(); p.provider=provider->currentText(); p.model=model->text().trimmed(); p.apiKey=key->text(); p.endpoint=endpoint->text().trimmed(); p.systemPrompt=prompt->toPlainText(); if(p.name.isEmpty()||p.model.isEmpty()||p.apiKey.isEmpty()){QMessageBox::warning(this,"Nexora","Nome, modelo e API key são obrigatórios.");return;}
    m_profiles.append(p); m_agent.setProfile(p); m_aiSelector->clear(); for(const auto &x:m_profiles)m_aiSelector->addItem(x.name+"  ·  "+x.model); showWorkspace();
}

void MainWindow::addAI(){configureFirstAI();}
void MainWindow::sendPrompt(){
    const QString prompt=m_prompt->toPlainText().trimmed(); if(prompt.isEmpty())return;
    if(!hasAI()){configureFirstAI(); if(!hasAI())return;}
    m_chat->appendPlainText("You  ·  "+prompt);
    if(!m_bridge.running()){m_chat->appendPlainText("Nexora  ·  Studio bridge is unavailable.");return;}
    const auto plan=m_agent.planLocalTools(prompt); for(const auto &item:plan){auto o=item.toObject();m_bridge.enqueue(o.value("command").toString(),o.value("args").toObject());m_chat->appendPlainText("Nexora  ·  "+o.value("command").toString()+" queued");} m_prompt->clear();
}
void MainWindow::bridgeState(bool connected){if(m_status)m_status->setText(connected?"●  Studio connected":"○  Studio disconnected");}
