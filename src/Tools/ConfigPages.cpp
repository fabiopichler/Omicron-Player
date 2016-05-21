/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "ConfigPages.h"
#include "../Core/RadioStream.h"
#include "../Gui/TitleBar.h"
#include "../Core/Theme.h"

#include <QtWidgets>
#include <bass.h>

//================================================================================================================
// GeneralPage
//================================================================================================================
GeneralPage::GeneralPage(QWidget *parent) : Widget(parent)
{
    setObjectName("configPagesWidget");

    QCheckBox *autoChBox = new QCheckBox("Reproduzir automaticamente ao iniciar o player");
    autoChBox->setChecked(Database::value("Config", "autoPlay").toBool());

    //---------------------------------------
    QLabel *errorLabel = new QLabel("Notificar erros de reprodução:");
    errorCombo = new ComboBox;
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
InterfacePage::InterfacePage(QWidget *parent) : Widget(parent)
{
    setObjectName("configPagesWidget");

    QLabel *themeLabel = new QLabel("Tema:");
    themeCombo = new ComboBox;

    updateThemeList();

    QHBoxLayout *themeLayout = new QHBoxLayout;
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(themeCombo);

    //--------------------------------------------
    QLabel *styleLabel = new QLabel("Estilo:");
    styleCombo = new ComboBox;

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

    connect(themeCombo, SIGNAL(activated(int)), this, SLOT(changeTheme(int)));
    connect(styleCombo, SIGNAL(activated(int)), this, SLOT(changeStyle(int)));
    connect(installButton, &QPushButton::clicked, [=]() {
        QMessageBox::information(this, "Info", "Na versão atual do programa, não é possível instalar um novo tema. "
                                 "Esta funcionalidade, será implementana futuramente.");
    });
}

void InterfacePage::changeTheme(int index)
{
    Database::setValue("Config", "theme", themeCombo->itemData(index));
    Database::setValue("Config", "style", "default");
    Theme::load();
    updateStyleList();
    styleCombo->setCurrentIndex(0);
}

void InterfacePage::changeStyle(int index)
{
    Database::setValue("Config", "style", styleCombo->itemData(index));
    Theme::load();
}

void InterfacePage::updateThemeList()
{
    QList<QStringList> themes = Theme::themes();
    QString current = Database::value("Config", "theme").toString();
    themeCombo->clear();

    for (int i = 0; i < themes.length(); i++)
    {
        themeCombo->addItem(themes[i][0], themes[i][1]);
        if (current == themes[i][1])
            themeCombo->setCurrentIndex(i);
    }
}

void InterfacePage::updateStyleList()
{
    QList<QStringList> styles = Theme::styles();
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
MusicPage::MusicPage(QWidget *parent) : Widget(parent)
{
    setObjectName("configPagesWidget");

    QCheckBox *notificationsChBox = new QCheckBox("Exibir notificações de reproduãço na bandeja do sistema");
    notificationsChBox->setChecked(Database::value("Config", "music_notifiSysTray").toBool());

    connect(notificationsChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("Config", "music_notifiSysTray", arg);
    });

    //---------------------------------------------------

    QCheckBox *continuePlayingChBox = new QCheckBox("Ao iniciar, continuar reproduzindo de onde parou");
    continuePlayingChBox->setChecked(Database::value("Config", "continuePlaying").toBool());

    connect(continuePlayingChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("Config", "continuePlaying", arg);
        if (Database::value("MusicMode", "soundPosition").toInt() != 0)
            Database::setValue("MusicMode", "soundPosition", 0);
    });

    //----------------------------------------------------

    QCheckBox *allowAnyFileChBox = new QCheckBox("Abrir arquivos desconhecidos com \"Arrastar e Soltar\"");
    allowAnyFileChBox->setChecked(Database::value("Config", "allowAnyFile").toBool());

    connect(allowAnyFileChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("Config", "allowAnyFile", arg);
    });

    //----------------------------------------------------

    QCheckBox *continuePlayTabChBox = new QCheckBox("Continuar reproduzindo ao trocar de aba");
    continuePlayTabChBox->setChecked(Database::value("Config", "continuePlayingTab").toBool());

    connect(continuePlayTabChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("Config", "continuePlayingTab", arg);
    });

    //------------------------------------------------------

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(notificationsChBox);
    mainLayout->addWidget(continuePlayingChBox);
    mainLayout->addWidget(allowAnyFileChBox);
    mainLayout->addWidget(continuePlayTabChBox);
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

//================================================================================================================
// WebRadioPage
//================================================================================================================
WebRadioPage::WebRadioPage(QWidget *parent) : Widget(parent)
{
    setObjectName("configPagesWidget");

    QCheckBox *notificationsChBox = new QCheckBox("Exibir notificações de reproduãço na bandeja do sistema");
    notificationsChBox->setChecked(Database::value("Config", "radio_notifiSysTray").toBool());

    connect(notificationsChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("Config", "radio_notifiSysTray", arg);
    });

    //-----------------------------------------------

    QLabel *readtimeoutLabel = new QLabel("Tempo limite ao reconectar:");

    readtimeoutCombo = new ComboBox;
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

    timeoutCombo = new ComboBox;
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

    modeCombo = new ComboBox;
    modeCombo->addItem("Parar a reprodução");
    modeCombo->addItem("Tentar reproduzir a mesma rádio");
    modeCombo->addItem("Reproduzir a próxima da lista");

    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->addWidget(modeLabel);
    modeLayout->addWidget(modeCombo);

    int list[6] = {5000, 10000, 20000, 30000, 45000, 60000};
    int rt = Database::value("Config", "net_readtimeout", 20000).toInt();
    int t = Database::value("Config", "net_timeout", 20000).toInt();

    for (int i = 0; i < 6; i++)
    {
        if (rt == list[i])
            readtimeoutCombo->setCurrentIndex(i);

        if (t == list[i])
            timeoutCombo->setCurrentIndex(i);
    }

    modeCombo->setCurrentIndex(Database::value("Config", "radioMode").toInt());

    connect(readtimeoutCombo, SIGNAL(activated(int)), this, SLOT(readtimeoutChanged()));
    connect(timeoutCombo, SIGNAL(activated(int)), this, SLOT(timeoutChanged()));
    connect(modeCombo, SIGNAL(activated(int)), this, SLOT(modeChanged()));

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(timeoutLayout);
    configLayout->addLayout(readtimeoutLayout);

    QGroupBox *configGroup = new QGroupBox("Conexão à Internet");
    configGroup->setLayout(configLayout);

    QVBoxLayout *configLayout2 = new QVBoxLayout;
    configLayout2->addLayout(modeLayout);

    QGroupBox *configGroup2 = new QGroupBox("Lista de Reprodução");
    configGroup2->setLayout(configLayout2);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(notificationsChBox);
    mainLayout->addWidget(configGroup);
    mainLayout->addWidget(configGroup2);
    mainLayout->addStretch(100);
    setLayout(mainLayout);
}

void WebRadioPage::readtimeoutChanged()
{
    BASS_SetConfig(BASS_CONFIG_NET_READTIMEOUT, readtimeoutCombo->currentData().toInt());
    Database::setValue("Config", "net_readtimeout", readtimeoutCombo->currentData());
}

void WebRadioPage::timeoutChanged()
{
    BASS_SetConfig(BASS_CONFIG_NET_TIMEOUT, timeoutCombo->currentData().toInt());
    Database::setValue("Config", "net_timeout", timeoutCombo->currentData());
}

void WebRadioPage::modeChanged()
{
    Database::setValue("Config", "radioMode", modeCombo->currentIndex());
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

FileTypePage::FileTypePage(QWidget *parent) : Widget(parent)
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
AudioPage::AudioPage(QObject *parentMain, QWidget *parent) : Widget(parent)
{
    this->parentMain = parentMain;
    setObjectName("configPagesWidget");

    QGroupBox *configGroup = new QGroupBox("Dispositivo de saída de áudio");

    ComboBox *deviceCombo = new ComboBox;
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
RecordingsPage::RecordingsPage(QWidget *parent) : Widget(parent)
{
    setObjectName("configPagesWidget");  

    QLabel *recLabel = new QLabel("Destino das gravações:");
    QLabel *defaultLabel = new QLabel("<a href=\"0\">Ou redefina para o diretório padrão</a>");

    pathEdit = new QLineEdit;

    recButton = new QPushButton("...");

    recButton->setStyleSheet("min-width: 1px;");
    pathEdit->setText(Database::value("Config", "recordPath").toString());

    QHBoxLayout *recLayout = new QHBoxLayout;
    recLayout->addWidget(pathEdit);
    recLayout->addWidget(recButton);

    QCheckBox *recChBox = new QCheckBox("Criar sub-pastas para cada rádio, separadamente");
    recChBox->setChecked(Database::value("Config", "recordSubDir", false).toBool());

    connect(recButton, SIGNAL(clicked()), this, SLOT(searchDir()));
    connect(defaultLabel, &QLabel::linkActivated, [=]() {
        QString path = QStandardPaths::writableLocation(QStandardPaths::MusicLocation)+"/Recordings - "+AppName+"/";
        Database::setValue("Config", "recordPath", path);
        pathEdit->setText(path);
    });
    connect(recChBox, &QCheckBox::clicked, [=](bool arg) {
        Database::setValue("Config", "recordSubDir", arg);
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
    QString path = QFileDialog::getExistingDirectory(this, "Open Directory", Database::value("Config", "recordPath").toString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!path.isEmpty())
    {
        Database::setValue("Config", "recordPath", path);
        pathEdit->setText(path);
    }
}

//================================================================================================================
// NetworkPage
//================================================================================================================
NetworkPage::NetworkPage(QWidget *parent) : Widget(parent)
{
    setObjectName("configPagesWidget");

    QLineEdit *proxyEdit = new QLineEdit;

    if (Database::value("Config", "net_proxy").toString() != "0")
        proxyEdit->setText(Database::value("Config", "net_proxy").toString());

    QPushButton *proxyButton = new QPushButton("Salvar");

    QHBoxLayout *configLayout = new QHBoxLayout;
    configLayout->addWidget(proxyEdit);
    configLayout->addWidget(proxyButton);

    connect(proxyButton, &QPushButton::clicked, [=]() {
        QString text = proxyEdit->text();
        Database::setValue("Config", "net_proxy", (text.isEmpty() ? "0" : text));
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
UpdatePage::UpdatePage(QWidget *parent) : Widget(parent)
{
    setObjectName("configPagesWidget");

    QLabel *infoLabel = new QLabel("Ao manter a verificação de atualizações ativada,<br>será possível que o programa, automaticamente:"
                                   "<ul><li>Verifique se existe uma nova versão disponível,<br>com melhorias e correções de erros.<br></li>"
                                   "<li>Atualize a base de estações de rádio (lista padrão),<br>caso exista uma nova atualização disponível.</li></ul>");
    QLabel *updateLabel = new QLabel("Verificar a cada:");

    ComboBox *updateCombo = new ComboBox;
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

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(configGroup);
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
