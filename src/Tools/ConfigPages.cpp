/*******************************************************************************

  Omicron Player Classic

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Player Classic nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "ConfigPages.h"
#include "../Core/RadioStream.h"
#include "../Core/MusicStream.h"

#include <OmicronTK/Qt/TitleBar.hpp>

#include <OmicronTK/Qt/Theme.hpp>

#include <QtWidgets>
#include <bass.h>

using namespace OmicronTK;

//================================================================================================================
// GeneralPage
//================================================================================================================
GeneralPage::GeneralPage(QWidget *parent) : OTKQT::Widget(parent)
{
    setObjectName("configPagesWidget");

    QCheckBox *autoChBox = new QCheckBox("Reproduzir automaticamente ao iniciar o player");
    autoChBox->setChecked(Database::value("Config", "autoPlay").toBool());

    //---------------------------------------
    QLabel *errorLabel = new QLabel("Notificar erros de reprodução:");
    errorCombo = new OTKQT::ComboBox;
    QString current = Database::value("Config", "errorNotification").toString();
    const char *list[] = {"dialog", "systray", "false"};

    errorCombo->addItem("Com uma caixa de diálogo", list[0]);
    errorCombo->addItem("Com a bandeja do Sistema", list[1]);
    errorCombo->addItem("Não notificar erros", list[2]);

    for (int i = 0; i < 3; i++)
        if (current == list[i])
            errorCombo->setCurrentIndex(i);

    QHBoxLayout *errorLayout = new QHBoxLayout;
    errorLayout->addWidget(errorLabel);
    errorLayout->addWidget(errorCombo);

    //--------------------------------------
    connect(autoChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("Config", "autoPlay", arg);
    });

    connect(errorCombo, SIGNAL(activated(int)), this, SLOT(changeError(int)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(autoChBox);
    mainLayout->addLayout(errorLayout);
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

void GeneralPage::changeError(int index)
{
    Database::setValue("Config", "errorNotification", errorCombo->itemData(index));
}

//================================================================================================================
// InterfacePage
//================================================================================================================
InterfacePage::InterfacePage(QWidget *parent) : OTKQT::Widget(parent)
{
    setObjectName("configPagesWidget");
    currentTheme = 0;

    QLabel *themeLabel = new QLabel("Tema:");
    themeCombo = new OTKQT::ComboBox;

    updateThemeList();

    QHBoxLayout *themeLayout = new QHBoxLayout;
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo);

    //--------------------------------------------
    QLabel *styleLabel = new QLabel("Estilo:");
    styleCombo = new OTKQT::ComboBox;

    updateStyleList();

    QHBoxLayout *colorLayout = new QHBoxLayout;
    colorLayout->addWidget(styleLabel);
    colorLayout->addWidget(styleCombo);

    //--------------------------------------------
    QLabel *installLabel = new QLabel("Instalar novo Tema:");
    QPushButton *installButton = new QPushButton("Procurar");

    QHBoxLayout *installLayout = new QHBoxLayout;
    installLayout->addWidget(installLabel);
    installLayout->addWidget(installButton);

    //--------------------------------------------
    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(themeLayout);
    configLayout->addLayout(colorLayout);

    QGroupBox *configGroup = new QGroupBox("Temas e Estilos");
    configGroup->setLayout(configLayout);

    QGroupBox *installGroup = new QGroupBox("Gerenciar Temas");
    installGroup->setLayout(installLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configGroup);
    mainLayout->addWidget(installGroup);
    mainLayout->addStretch(100);
    setLayout(mainLayout);

    connect(this, SIGNAL(restartApp()), parent, SIGNAL(restartApp()));
    connect(themeCombo, SIGNAL(activated(int)), this, SLOT(changeTheme(int)));
    connect(styleCombo, SIGNAL(activated(int)), this, SLOT(changeStyle(int)));
    connect(installButton, &QPushButton::clicked, [=]() {
        QMessageBox::information(this, "Info", "Na versão atual do programa, não é possível instalar um novo tema. "
                                 "Esta funcionalidade, será implementana futuramente.");
    });
}

void InterfacePage::changeTheme(int index)
{
    if (QMessageBox::information(this, "Alteração do tema", "É necessário reiniciar o programa "
                                 "para concluir a alteração do tema.\n\nAlterar o tema e reiniciar o programa?",
                                 QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
    {
        Database::setValue("Config", "theme", themeCombo->itemData(index));
        Database::setValue("Config", "style", "default");
        emit restartApp();
    }
    else
    {
        themeCombo->setCurrentIndex(currentTheme);
    }
}

void InterfacePage::changeStyle(int index)
{
    Database::setValue("Config", "style", styleCombo->itemData(index));
    OTKQT::Theme::load();
}

void InterfacePage::updateThemeList()
{
    QVector<QVector<QString>> themes = OTKQT::Theme::themes();
    QString current = Database::value("Config", "theme").toString();
    themeCombo->clear();

    for (int i = 0; i < themes.length(); i++)
    {
        themeCombo->addItem(themes[i][0], themes[i][1]);

        if (current == themes[i][1])
        {
            currentTheme = i;
            themeCombo->setCurrentIndex(i);
        }
    }
}

void InterfacePage::updateStyleList()
{
    QVector<QVector<QString>> styles = OTKQT::Theme::styles();
    QString current = Database::value("Config", "style").toString();
    styleCombo->clear();

    for (int i = 0; i < styles.length(); i++)
    {
        styleCombo->addItem(styles[i][0], styles[i][1]);
        if (current == styles[i][1])
            styleCombo->setCurrentIndex(i);
    }
}

//================================================================================================================
// MusicPage
//================================================================================================================
MusicPage::MusicPage(QWidget *parent) : OTKQT::Widget(parent)
{
    setObjectName("configPagesWidget");

    QCheckBox *agcChBox = new QCheckBox("Ativar o controle automático de ganho (AGC)");
    agcChBox->setChecked(Database::value("MusicConfig", "automaticGainControl").toBool());

    connect(agcChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("MusicConfig", "automaticGainControl", arg);

        MusicStream *musicStream = MusicStream::instance();

        if (musicStream)
            musicStream->automaticGainControl(arg);
    });

    //---------------------------------------------------

    QCheckBox *notificationsChBox = new QCheckBox("Exibir notificações de reproduãço na bandeja do sistema");
    notificationsChBox->setChecked(Database::value("MusicConfig", "notifiSysTray").toBool());

    connect(notificationsChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("MusicConfig", "notifiSysTray", arg);
    });

    //---------------------------------------------------

    QCheckBox *continuePlayingChBox = new QCheckBox("Ao iniciar, continuar reproduzindo de onde parou");
    continuePlayingChBox->setChecked(Database::value("MusicConfig", "continuePlaying").toBool());

    connect(continuePlayingChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("MusicConfig", "continuePlaying", arg);
        if (Database::value("MusicMode", "soundPosition").toInt() != 0)
            Database::setValue("MusicMode", "soundPosition", 0);
    });

    //----------------------------------------------------

    QCheckBox *allowAnyFileChBox = new QCheckBox("Abrir arquivos desconhecidos com \"Arrastar e Soltar\"");
    allowAnyFileChBox->setChecked(Database::value("MusicConfig", "allowAnyFile").toBool());

    connect(allowAnyFileChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("MusicConfig", "allowAnyFile", arg);
    });

    //----------------------------------------------------

    QCheckBox *continuePlayTabChBox = new QCheckBox("Continuar reproduzindo ao trocar de aba");
    continuePlayTabChBox->setChecked(Database::value("MusicConfig", "continuePlayingTab").toBool());

    connect(continuePlayTabChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("MusicConfig", "continuePlayingTab", arg);
    });

    //------------------------------------------------------

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(agcChBox);
    mainLayout->addWidget(notificationsChBox);
    mainLayout->addWidget(continuePlayingChBox);
    mainLayout->addWidget(allowAnyFileChBox);
    mainLayout->addWidget(continuePlayTabChBox);
    mainLayout->addWidget(new FadeConfigBox("MusicConfig", "Transição das Faixas", this));
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

//================================================================================================================
// WebRadioPage
//================================================================================================================
WebRadioPage::WebRadioPage(QWidget *parent) : OTKQT::Widget(parent)
{
    setObjectName("configPagesWidget");

    QCheckBox *agcChBox = new QCheckBox("Ativar o controle automático de ganho (AGC)");
    agcChBox->setChecked(Database::value("RadioConfig", "automaticGainControl").toBool());

    connect(agcChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("RadioConfig", "automaticGainControl", arg);

        RadioStream *radioStream = RadioStream::instance();

        if (radioStream)
            radioStream->automaticGainControl(arg);
    });

    //---------------------------------------------------

    QCheckBox *notificationsChBox = new QCheckBox("Exibir notificações de reproduãço na bandeja do sistema");
    notificationsChBox->setChecked(Database::value("RadioConfig", "notifiSysTray").toBool());

    connect(notificationsChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("RadioConfig", "notifiSysTray", arg);
    });

    //-----------------------------------------------

    QLabel *readtimeoutLabel = new QLabel("Tempo limite ao reconectar:");

    readtimeoutCombo = new OTKQT::ComboBox;
    readtimeoutCombo->addItem("5 segundos", 5000);
    readtimeoutCombo->addItem("10 segundos", 10000);
    readtimeoutCombo->addItem("20 segundos (padrão)", 20000);
    readtimeoutCombo->addItem("30 segundos", 30000);
    readtimeoutCombo->addItem("45 segundos", 45000);
    readtimeoutCombo->addItem("60 segundos", 60000);

    QHBoxLayout *readtimeoutLayout = new QHBoxLayout;
    readtimeoutLayout->addWidget(readtimeoutLabel);
    readtimeoutLayout->addWidget(readtimeoutCombo);

    QLabel *timeoutLabel = new QLabel("Tempo limite ao conectar:");

    timeoutCombo = new OTKQT::ComboBox;
    timeoutCombo->addItem("5 segundos", 5000);
    timeoutCombo->addItem("10 segundos", 10000);
    timeoutCombo->addItem("20 segundos (padrão)", 20000);
    timeoutCombo->addItem("30 segundos", 30000);
    timeoutCombo->addItem("45 segundos", 45000);
    timeoutCombo->addItem("60 segundos", 60000);

    QHBoxLayout *timeoutLayout = new QHBoxLayout;
    timeoutLayout->addWidget(timeoutLabel);
    timeoutLayout->addWidget(timeoutCombo);

    QLabel *modeLabel = new QLabel("Se a conexão cair:");

    modeCombo = new OTKQT::ComboBox;
    modeCombo->addItem("Parar a reprodução");
    modeCombo->addItem("Tentar reproduzir, novamente");
    modeCombo->addItem("Reproduzir a próxima estação da lista");

    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(modeCombo);

    int list[6] = {5000, 10000, 20000, 30000, 45000, 60000};
    int rt = Database::value("RadioConfig", "net_readtimeout", 20000).toInt();
    int t = Database::value("RadioConfig", "net_timeout", 20000).toInt();

    for (int i = 0; i < 6; i++)
    {
        if (rt == list[i])
            readtimeoutCombo->setCurrentIndex(i);

        if (t == list[i])
            timeoutCombo->setCurrentIndex(i);
    }

    modeCombo->setCurrentIndex(Database::value("RadioConfig", "reconnectionMode").toInt());

    connect(readtimeoutCombo, SIGNAL(activated(int)), this, SLOT(readtimeoutChanged()));
    connect(timeoutCombo, SIGNAL(activated(int)), this, SLOT(timeoutChanged()));
    connect(modeCombo, SIGNAL(activated(int)), this, SLOT(modeChanged()));

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(timeoutLayout);
    configLayout->addLayout(readtimeoutLayout);

    QGroupBox *configGroup = new QGroupBox("Conexão à Internet");
    configGroup->setLayout(configLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(agcChBox);
    mainLayout->addWidget(notificationsChBox);
    mainLayout->addLayout(modeLayout);
    mainLayout->addWidget(configGroup);
    mainLayout->addWidget(new FadeConfigBox("RadioConfig", "Transição de Rádios", this));
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

void WebRadioPage::readtimeoutChanged()
{
    BASS_SetConfig(BASS_CONFIG_NET_READTIMEOUT, readtimeoutCombo->currentData().toInt());
    Database::setValue("RadioConfig", "net_readtimeout", readtimeoutCombo->currentData());
}

void WebRadioPage::timeoutChanged()
{
    BASS_SetConfig(BASS_CONFIG_NET_TIMEOUT, timeoutCombo->currentData().toInt());
    Database::setValue("RadioConfig", "net_timeout", timeoutCombo->currentData());
}

void WebRadioPage::modeChanged()
{
    Database::setValue("RadioConfig", "reconnectionMode", modeCombo->currentIndex());
}

//================================================================================================================
// FileTypePage
//================================================================================================================
#ifdef Q_OS_WIN

FileTypeTree::FileTypeTree(QWidget *parent) : QTreeView(parent)
{
    this->parent = parent;

    model = new QStandardItemModel(0, 2);
    model->setHeaderData(0, Qt::Horizontal, "Arquivos");
    model->setHeaderData(1, Qt::Horizontal, "Tipo");

    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setEditTriggers(NoEditTriggers);
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectRows);
    setIndentation(0);
    setModel(model);

}

FileTypeTree::~FileTypeTree()
{
    delete model;
}

void FileTypeTree::addRow(const QStringList &files, bool checked)
{
    QList<QStandardItem *> item;

    item << new QStandardItem(files[0]);
    item << new QStandardItem(files[1]);

    item[0]->setCheckable(true);
    item[0]->setEnabled(!checked);
    item[1]->setEnabled(!checked);
    if (checked)
        item[0]->setCheckState(Qt::Checked);

    model->appendRow(item);
}

QString FileTypeTree::getChecked()
{
    QString str;
    int len = model->rowCount();

    for (int i = 0; i < len; i++)
        if (model->item(i)->checkState() == Qt::Checked)
            str += model->data(model->index(i, 0)).toString() + "|";

    return str.remove(QRegExp("[|]$"));
}

void FileTypeTree::clear()
{
    if (model->hasChildren())
        model->removeRows(0, model->rowCount());
}

void FileTypeTree::selectAllRows()
{
    int len = model->rowCount();

    for (int i = 0; i < len; i++)
        model->item(i)->setCheckState(Qt::Checked);
}

//------------------------------------------------------------------------------------------

FileTypePage::FileTypePage(QWidget *parent) : OTKQT::Widget(parent)
{
    setObjectName("configPagesWidget");

    QLabel *label = new QLabel("Escolha os arquivos para associar ao programa.");
    QPushButton *selectButton = new QPushButton("Selecionar Tudo");
    QPushButton *button = new QPushButton("Associar Arquivos");

    fileTypeTree = new FileTypeTree(this);
    fileAssoc = new FileAssociation(QApplication::applicationFilePath());

    updateTreeView();

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(selectButton);
    buttonLayout->addWidget(button);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(fileTypeTree, 1);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(selectButton, SIGNAL(clicked()), fileTypeTree, SLOT(selectAllRows()));
    connect(button, SIGNAL(clicked()), this, SLOT(associateFiles()));
}

FileTypePage::~FileTypePage()
{
    delete fileAssoc;
}

void FileTypePage::updateTreeView()
{
    fileTypeTree->clear();
    for (int i = 0; i < Global::supportedFiles.length(); i++)
    {
        QStringList files = Global::supportedFiles[i];
        fileTypeTree->addRow(files, fileAssoc->isDefault(files[0]));
    }
}

void FileTypePage::associateFiles()
{
    QList<QStringList> supportedFiles;
    QString checked = fileTypeTree->getChecked();

    if (checked.isEmpty())
    {
        QMessageBox::warning(this, "Aviso", "Nemhum arquivo selecionado.");
        return;
    }

    for (int i = 0; i < Global::supportedFiles.length(); i++)
        if (Global::supportedFiles[i][0].contains(QRegExp(checked)))
            supportedFiles << Global::supportedFiles[i];

    QStringList errors = fileAssoc->registerList(supportedFiles, true);
    updateTreeView();

    if (errors.length() == 0)
    {
        QMessageBox::information(this, "Sucesso", "Os arquivos foram associados (ou atualizados) com êxito.");
    }
    else
    {
        QString errorMsg = "Alguns arquivos não puderam ser associados ao programa, "
                           "terá que fazê-los manualmente:\n";

        for (int i = 0; i < errors.length(); i++)
            errorMsg += errors[i] + "\n";

        QMessageBox::warning(this, "Aviso", errorMsg);
    }
}

#endif // Q_OS_WIN

//================================================================================================================
// AudioPage
//================================================================================================================
AudioPage::AudioPage(QObject *parentMain, QWidget *parent) : OTKQT::Widget(parent)
{
    this->parentMain = parentMain;
    setObjectName("configPagesWidget");

    QGroupBox *configGroup = new QGroupBox("Dispositivo de saída de áudio");

    OTKQT::ComboBox *deviceCombo = new OTKQT::ComboBox;
    deviceCombo->addItem("Padrão do Sistema");

    int device = Database::value("Config", "device").toInt();
    BASS_DEVICEINFO deviceInfo;
    bool deviceOk = false;

    for (int i = 1; BASS_GetDeviceInfo(i, &deviceInfo); i++)
    {
        if (deviceInfo.flags&BASS_DEVICE_ENABLED)
        {
            deviceCombo->addItem(deviceInfo.name);
            if (device == i)
                deviceOk = true;
        }
    }

    deviceCombo->setCurrentIndex(deviceOk ? device : 0);

    connect(deviceCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeDevice(int)));
    connect(this, SIGNAL(stopStream()), parentMain, SIGNAL(stopStream()));

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addWidget(deviceCombo);
    configGroup->setLayout(configLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configGroup);
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

void AudioPage::changeDevice(int arg)
{
    if (arg < 1)
        arg = -1;

    emit stopStream();
    Database::setValue("Config", "device", arg);

    BASS_Stop();
    BASS_Free();

    if (!BASS_Init(arg,44100,0,0,nullptr))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Não foi possível iniciar o dispositivo "
                             "selecionado!<br>Verifique se o dispositivo de áudio está funcionando corretamente "
                             "e tente novamente."));
}

//================================================================================================================
// RecordingsPage
//================================================================================================================
RecordingsPage::RecordingsPage(QWidget *parent) : OTKQT::Widget(parent)
{
    setObjectName("configPagesWidget");  

    QLabel *recLabel = new QLabel("Destino das gravações:");
    QLabel *defaultLabel = new QLabel("<a href=\"0\">Ou redefina para o diretório padrão</a>");

    pathEdit = new QLineEdit;

    recButton = new QPushButton("...");

    recButton->setStyleSheet("min-width: 1px;");
    pathEdit->setText(Database::value("RadioConfig", "recordPath").toString());

    QHBoxLayout *recLayout = new QHBoxLayout;
    recLayout->addWidget(pathEdit);
    recLayout->addWidget(recButton);

    QCheckBox *recChBox = new QCheckBox("Criar sub-pastas para cada rádio, separadamente");
    recChBox->setChecked(Database::value("RadioConfig", "recordSubDir", false).toBool());

    connect(recButton, SIGNAL(clicked()), this, SLOT(searchDir()));
    connect(defaultLabel, &QLabel::linkActivated, [=]() {
        QString path = QStandardPaths::writableLocation(QStandardPaths::MusicLocation)+"/"+AppName+" Recordings/";
        Database::setValue("RadioConfig", "recordPath", path);
        pathEdit->setText(path);

        if (!QDir().exists(path))
            QDir().mkpath(path);
    });
    connect(recChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("RadioConfig", "recordSubDir", arg);
    });

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addWidget(recLabel);
    configLayout->addLayout(recLayout);
    configLayout->addWidget(defaultLabel,0,Qt::AlignRight);
    configLayout->addWidget(recChBox);

    QGroupBox *configGroup = new QGroupBox("Diretório de gravação");
    configGroup->setLayout(configLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configGroup);
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

void RecordingsPage::searchDir()
{
    QString path = QFileDialog::getExistingDirectory(this, "Open Directory", Database::value("RadioConfig", "recordPath").toString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!path.isEmpty())
    {
        Database::setValue("RadioConfig", "recordPath", path);
        pathEdit->setText(path);
    }
}

//================================================================================================================
// NetworkPage
//================================================================================================================
NetworkPage::NetworkPage(QWidget *parent) : OTKQT::Widget(parent)
{
    setObjectName("configPagesWidget");

    QLineEdit *proxyEdit = new QLineEdit;

    if (Database::value("RadioConfig", "net_proxy").toString() != "0")
        proxyEdit->setText(Database::value("RadioConfig", "net_proxy").toString());

    QPushButton *proxyButton = new QPushButton("Salvar");

    QHBoxLayout *configLayout = new QHBoxLayout;
    configLayout->addWidget(proxyEdit);
    configLayout->addWidget(proxyButton);

    connect(proxyButton, &QPushButton::clicked, [=]() {
        QString text = proxyEdit->text();
        Database::setValue("RadioConfig", "net_proxy", (text.isEmpty() ? "0" : text));
        BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, (text.isEmpty() ? NULL : text.toLocal8Bit().data()));
    });

    QGroupBox *configGroup = new QGroupBox("Configuração de Proxy");
    configGroup->setLayout(configLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configGroup);
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

//================================================================================================================
// UpdatePage
//================================================================================================================
UpdatePage::UpdatePage(QWidget *parent) : OTKQT::Widget(parent)
{
    setObjectName("configPagesWidget");

    QLabel *infoLabel = new QLabel("Ao manter a verificação de atualizações ativada,<br>será possível que o programa, automaticamente:"
                                   "<ul><li>Verifique se existe uma nova versão disponível,<br>com melhorias e correções de erros.<br></li>"
                                   "<li>Atualize a base de estações de rádio (lista padrão),<br>caso exista uma nova atualização disponível.</li></ul>");
    QLabel *updateLabel = new QLabel("Verificar a cada:");

    OTKQT::ComboBox *updateCombo = new OTKQT::ComboBox;
    updateCombo->addItem("Desativado");
    updateCombo->addItem("1 dia");
    updateCombo->addItem("1 semana");

    QString value = Database::value("Version", "updates_check", "1").toString();

    if (value == "0")
        updateCombo->setCurrentIndex(0);
    else if (value == "2")
        updateCombo->setCurrentIndex(2);
    else
        updateCombo->setCurrentIndex(1);

    connect(updateCombo, SIGNAL(activated(int)), this, SLOT(changeUpdade(int)));

    QHBoxLayout *updateLayout = new QHBoxLayout;
    updateLayout->addWidget(updateLabel);
    updateLayout->addWidget(updateCombo);

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(updateLayout);

    QGroupBox *configGroup = new QGroupBox("Verificação");
    configGroup->setLayout(configLayout);

    //--------------------------------------------
    QCheckBox *radioListCheckBox = new QCheckBox("Atualizar base de estações de rádio, automaticamente.");

    radioListCheckBox->setChecked(Database::value("Config", "autoDlRadioList").toBool());

    connect(radioListCheckBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("Config", "autoDlRadioList", arg);
    });

    QLabel *radioListLabel = new QLabel("<strong>Nota:</strong> Para que a base de rádios, possa ser atualizada,<br>"
                                   "é preciso que a verificação de atualizações, esteja ativada.");

    //--------------------------------------------
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(configGroup);
    mainLayout->addWidget(radioListCheckBox);
    mainLayout->addWidget(radioListLabel);
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

void UpdatePage::changeUpdade(int index)
{
    if (index == 0)
        Database::setValue("Version", "updates_check", 0);
    else if (index == 2)
        Database::setValue("Version", "updates_check", 2);
    else
        Database::setValue("Version", "updates_check", 1);
}

//================================================================================================================
// FadeConfigBox
//================================================================================================================

FadeConfigBox::FadeConfigBox(const QString &name, const QString &title, QWidget *parent)
                                             : QGroupBox(title, parent), configName(name)
{
    QLabel *infoLabel = new QLabel("<strong>Nota:</strong> Para desativar as transições, deixe com<br> valores 0 (zero)."
                                   " O tempo máximo é 30 seg.");
    QLabel *fadeInLabel = new QLabel("Tempo para o <strong>Fade in</strong> (em segundos)");
    QSpinBox *fadeInSpinBox = new QSpinBox;

    fadeInSpinBox->setRange(0, 30);
    fadeInSpinBox->setValue(Database::value(configName, "fadeIn", 0).toInt());

    QHBoxLayout *fadeInLayout = new QHBoxLayout;
    fadeInLayout->addWidget(fadeInLabel);
    fadeInLayout->addWidget(fadeInSpinBox);

    //--------------------------------------------
    QLabel *fadeOutLabel = new QLabel("Tempo para o <strong>Fade out</strong> (em segundos)");
    QSpinBox *fadeOutSpinBox = new QSpinBox;

    fadeOutSpinBox->setRange(0, 30);
    fadeOutSpinBox->setValue(Database::value(configName, "fadeOut", 0).toInt());

    QHBoxLayout *fadeOutLayout = new QHBoxLayout;
    fadeOutLayout->addWidget(fadeOutLabel);
    fadeOutLayout->addWidget(fadeOutSpinBox);

    //--------------------------------------------
    QPushButton *saveButton = new QPushButton("Salvar");

    connect(saveButton, &QCheckBox::clicked, [=]() {
        Database::setValue(configName, "fadeIn", fadeInSpinBox->value());
        Database::setValue(configName, "fadeOut", fadeOutSpinBox->value());
        QMessageBox::information(this, "Info", "Salvo con sucesso.");
    });

    //--------------------------------------------
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addLayout(fadeInLayout);
    vLayout->addLayout(fadeOutLayout);
    vLayout->addWidget(infoLabel);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(vLayout);
    mainLayout->addWidget(saveButton, 0, Qt::AlignRight);
    setLayout(mainLayout);
}
