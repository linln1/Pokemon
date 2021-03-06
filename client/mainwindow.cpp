#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QTime>
#include <QHostAddress>
#include <QDir>

#include "netconfig.h"
#include "pokemondlg.h"
#include "authentication.h"
#include<QHeaderView>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow)
{
	ui->setupUi(this);


    setWindowTitle(tr("Pokemon Dual"));

	// start layout
    lbStartTitle = new QLabel(tr("Pokemon Dual"), this);
	lbStartTitle->setObjectName("lbStartTitle");
	btnPlay = new QPushButton(tr("开始游戏"), this);
	btnPlay->setObjectName("btnPlay");
    btnPlay->setIcon(QPixmap(": img/play.png"));
	btnExit = new QPushButton(tr("退出"), this);
	btnExit->setObjectName("btnExit");
    btnExit->setIcon(QPixmap("img/exit.png"));

	// login layout
	lbLoginLabel = new QLabel(tr("登录"), this);
	lbLoginLabel->setObjectName("lbLoginLabel");
	leUsername = new QLineEdit(this);
	leUsername->setObjectName("leUsername");
	leUsername->setPlaceholderText(tr("请输入用户名"));
	lePassword = new QLineEdit(this);
	lePassword->setObjectName("lePassword");
	lePassword->setPlaceholderText(tr("请输入密码"));
	lePassword->setEchoMode(QLineEdit::Password);
	btnLogin = new QPushButton(tr("登录"), this);
	btnLogin->setObjectName("btnLogin");
	btnLogon = new QPushButton(tr("注册"), this);
	btnLogon->setObjectName("btnLogon");
	btnBack = new QPushButton(tr("返回"), this);
	btnBack->setObjectName("btnBack");

	// main layout
	btnShowPokemonList = new QPushButton(tr("查看精灵"), this);
	btnShowPokemonList->setObjectName("btnShowPokemonList");
	btnLvUpBattle = new QPushButton(tr("升级比赛"), this);
	btnLvUpBattle->setObjectName("btnLvUpBattle");
	btnDuelBattle = new QPushButton(tr("决斗比赛"), this);
	btnDuelBattle->setObjectName("btnDuelBattle");
	btnDisplayAllPlayer = new QPushButton(tr("查看所有玩家"), this);
	btnDisplayAllPlayer->setObjectName("btnDisplayAllPlayer");
	btnChangePassword = new QPushButton(tr("修改密码"), this);
	btnChangePassword->setObjectName("btnChangePassword");
	btnLogout = new QPushButton(tr("退出登录"), this);
	btnLogout->setObjectName("btnLogout");

	// change password layout
	leNewPassword = new QLineEdit(this);
	leNewPassword->setObjectName("leNewPassword");
	btnOK = new QPushButton(tr("提交"), this);
	btnOK->setObjectName("btnOK");
	leNewPassword->setPlaceholderText(tr("请输入新密码"));
	leNewPassword->setEchoMode(QLineEdit::Password);

	// duel battle statistic layout
	lbWin = new QLabel(this);
	lbWin->setObjectName("lbWin");
	lbTotal = new QLabel(this);
	lbTotal->setObjectName("lbTotal");
	lbWinRate = new QLabel(this);
	lbWinRate->setObjectName("lbWinRate");

	// choose enemy layout
	lbChooseEnemy = new QLabel(tr("选择你的对手："), this);
	lbChooseEnemy->setObjectName("lbChooseEnemy");
	lbEnemyLV = new QLabel(tr("对手等级："), this);
	lbEnemyLV->setObjectName("lbEnemyLV");
	sbEnemyLV = new QSpinBox(this);
	sbEnemyLV->setObjectName("sbEnemyLV");
    btnEnemyRace0 = new QPushButton(tr("小火猴"), this);
	btnEnemyRace0->setObjectName("btnEnemyRace0");
    btnEnemyRace0->setIcon(QPixmap("img/xiaohuohou.png"));
    btnEnemyRace0->setIconSize(QSize(300, 300));
    btnEnemyRace1 = new QPushButton(tr("树苗龟"), this);
	btnEnemyRace1->setObjectName("btnEnemyRace1");
    btnEnemyRace1->setIcon(QPixmap("img/shumiaogui.png"));
    btnEnemyRace1->setIconSize(QSize(300, 300));
    btnEnemyRace2 = new QPushButton(tr("莫比"), this);
	btnEnemyRace2->setObjectName("btnEnemyRace2");
    btnEnemyRace2->setIcon(QPixmap("img/mobi.jpg"));
	btnEnemyRace2->setIconSize(QSize(300, 300));
    btnEnemyRace3 = new QPushButton(tr("波克尔"), this);
	btnEnemyRace3->setObjectName("btnEnemyRace3");
    btnEnemyRace3->setIcon(QPixmap("img/pidgey.png"));
    btnEnemyRace3->setIconSize(QSize(300, 300));
	sbEnemyLV->setMaximum(15);
	sbEnemyLV->setMinimum(1);

	// battle layout
	lbP1 = new QLabel(tr("精灵1"), this);
	lbP2 = new QLabel(tr("精灵2"), this);
	btnSkill_1 = new QPushButton(this);
    btnSkill_1->setObjectName("btnSkill_1");
    btnSkill_1->setFixedSize(200, 120);
	btnSkill_2 = new QPushButton(this);
	btnSkill_2->setObjectName("btnSkill_2");
    btnSkill_2->setFixedSize(200, 120);
	btnSkill_3 = new QPushButton(this);
	btnSkill_3->setObjectName("btnSkill_3");
    btnSkill_3->setFixedSize(200, 120);
	btnSkill_4 = new QPushButton(this);
	btnSkill_4->setObjectName("btnSkill_4");
    btnSkill_4->setFixedSize(200, 120);

	pbP1HP = new QProgressBar(this);
	pbP1HP->setFormat("HP: %v/%m");

	pbP2HP = new QProgressBar(this);
	pbP2HP->setFormat("HP: %v/%m");

	lbP1SkillName = new QLabel(this);
	lbP1SkillName->setObjectName("lbP1SkillName");
	lbP2SkillName = new QLabel(this);
	lbP2SkillName->setObjectName("lbP2SkillName");

	// choose bet layout
	lbBet[1] = new QLabel(this);
	lbBet[2] = new QLabel(this);
	lbBet[0] = new QLabel(this);
	btnBet[1] = new QPushButton(this);
	btnBet[2] = new QPushButton(this);
	btnBet[0] = new QPushButton(this);

	// pokemon table and player table
	table = new QTableWidget(this);
	lbPokemonMasterBadge = new QLabel(this);
	lbPokemonNumBadge = new QLabel(this);
	lbPokemonMasterBadge->setFixedHeight(80);
	lbPokemonNumBadge->setFixedHeight(80);

	// logon window
	logonDlg = new LogonDlg(this);



    //qDebug()<<QDir::currentPath();


	connect(btnPlay, &QPushButton::clicked, this, [this] { changeState(LOGIN); });
	connect(btnExit, &QPushButton::clicked, this, [this] { qApp->quit(); });
	connect(btnLogin, &QPushButton::clicked, this, &MainWindow::login);
	connect(btnLogon, &QPushButton::clicked, this, [this] {
		if (logonDlg->exec() == QDialog::Accepted)
		{
			// auto set username and password if logon succeed
			leUsername->setText(logonDlg->getUsername());
			lePassword->setText(logonDlg->getPassword());
		}
	});
	connect(btnBack, &QPushButton::clicked, this, [this] {
		switch (state)
		{
		case LOGIN:
			changeState(START);
			client->disconnectFromHost();
			break;
		case POKEMON_TABLE:
			if (currentPlayerID == 0)
			{
				changeState(MAIN);
			}
			else
			{
				changeState(PLAYER_TABLE);
				client->write("getPlayerList", BUF_LENGTH);
			}
			break;
		case PLAYER_TABLE:
		case CHANGE_PSW:
		case POKEMON_TABLE | LV_UP_BATTLE:
		case POKEMON_TABLE | DUEL_BATTLE:
			changeState(MAIN);
			break;
		case CHOOSE_ENEMY | LV_UP_BATTLE:
		case CHOOSE_ENEMY | DUEL_BATTLE:
			changeState((state ^ CHOOSE_ENEMY) | POKEMON_TABLE);
			client->write("getPokemonList", BUF_LENGTH);
			break;
		case LV_UP_BATTLE:
		case DUEL_BATTLE:
			changeState(state | CHOOSE_ENEMY);
			break;
		default:
			break;
		}
	});



    connect(btnLogout, &QPushButton::clicked, this, [this] {//当按下logout按钮时，发送"logout"给server
		changeState(LOGIN);
		client->write("logout", BUF_LENGTH);
		client->disconnectFromHost();
	});
	connect(btnShowPokemonList, &QPushButton::clicked, this, [this] {
		currentPlayerID = 0;
		showPokemonDlg = false;
		changeState(POKEMON_TABLE);
		client->write("getPokemonList", BUF_LENGTH);
	});
	connect(btnLvUpBattle, &QPushButton::clicked, this, [this] {
		currentPlayerID = 0;
		client->write("getPokemonList", BUF_LENGTH);
		changeState(POKEMON_TABLE | LV_UP_BATTLE);
	});
	connect(btnDuelBattle, &QPushButton::clicked, this, [this] {
		currentPlayerID = 0;
		client->write("getPokemonList", BUF_LENGTH);
		changeState(POKEMON_TABLE | DUEL_BATTLE);
	});
	connect(btnDisplayAllPlayer, &QPushButton::clicked, this, [this] {
		changeState(PLAYER_TABLE);
		client->write("getPlayerList", BUF_LENGTH);
	});
	connect(btnChangePassword, &QPushButton::clicked, this, [this] { changeState(CHANGE_PSW); });


	connect(btnOK, &QPushButton::clicked, this, [this] {
		if (!isValidPassword(lePassword->text()))
		{
			QMessageBox::warning(this, tr("错误"), tr("旧密码格式错误"));
			lePassword->clear();
		}
		else if (!isValidPassword(leNewPassword->text()))
		{
			QMessageBox::warning(this, tr("错误"), tr("新密码格式错误"));
			leNewPassword->clear();
		}
		else
		{
			QString str = "resetPassword ";
			str += lePassword->text();
			str += ' ';
			str += leNewPassword->text();
			client->write(str.toStdString().c_str(), BUF_LENGTH);
		}
	});

	connect(btnEnemyRace0, &QPushButton::clicked, [this] {
		battleStart = true;
		QString msg = "battle ";
		if ((state & LV_UP_BATTLE) == LV_UP_BATTLE)
            msg += "LV_UP ";//升级赛
		else
            msg += "DUEL ";//决斗赛
		msg += battlePokemonID + ' ';
        msg += "0 ";    //enemyRaceID
		msg += QString::number(sbEnemyLV->value());
		client->write(msg.toLocal8Bit(), BUF_LENGTH);
		changeState(state ^ CHOOSE_ENEMY);
	});
	connect(btnEnemyRace1, &QPushButton::clicked, [this] {
		battleStart = true;
		QString msg = "battle ";
		if ((state & LV_UP_BATTLE) == LV_UP_BATTLE)
			msg += "LV_UP ";
		else
			msg += "DUEL ";
		msg += battlePokemonID + ' ';
		msg += "1 ";
		msg += QString::number(sbEnemyLV->value());
		client->write(msg.toLocal8Bit(), BUF_LENGTH);
		changeState(state ^ CHOOSE_ENEMY);
	});
	connect(btnEnemyRace2, &QPushButton::clicked, [this] {
		battleStart = true;
		QString msg = "battle ";
		if ((state & LV_UP_BATTLE) == LV_UP_BATTLE)
			msg += "LV_UP ";
		else
			msg += "DUEL ";
		msg += battlePokemonID + ' ';
		msg += "2 ";
		msg += QString::number(sbEnemyLV->value());
		client->write(msg.toLocal8Bit(), BUF_LENGTH);
		changeState(state ^ CHOOSE_ENEMY);
	});
	connect(btnEnemyRace3, &QPushButton::clicked, [this] {
		battleStart = true;
		QString msg = "battle ";
		if ((state & LV_UP_BATTLE) == LV_UP_BATTLE)
			msg += "LV_UP ";
		else
			msg += "DUEL ";
		msg += battlePokemonID + ' ';
		msg += "3 ";
		msg += QString::number(sbEnemyLV->value());
		client->write(msg.toLocal8Bit(), BUF_LENGTH);
		changeState(state ^ CHOOSE_ENEMY);
	});


    //当按动了某一技能的按钮时，将所有按钮设为disabled，即无法按动的状态
	connect(btnSkill_1, &QPushButton::clicked, [this] {
		client->write("0", BUF_LENGTH);
		btnSkill_1->setDisabled(true);
		btnSkill_2->setDisabled(true);
		btnSkill_3->setDisabled(true);
		btnSkill_4->setDisabled(true);
	});
	connect(btnSkill_2, &QPushButton::clicked, [this] {
		client->write("1", BUF_LENGTH);
		btnSkill_1->setDisabled(true);
		btnSkill_2->setDisabled(true);
		btnSkill_3->setDisabled(true);
		btnSkill_4->setDisabled(true);
	});
	connect(btnSkill_3, &QPushButton::clicked, [this] {
		client->write("2", BUF_LENGTH);
		btnSkill_1->setDisabled(true);
		btnSkill_2->setDisabled(true);
		btnSkill_3->setDisabled(true);
		btnSkill_4->setDisabled(true);
	});
	connect(btnSkill_4, &QPushButton::clicked, [this] {
		client->write("3", BUF_LENGTH);
		btnSkill_1->setDisabled(true);
		btnSkill_2->setDisabled(true);
		btnSkill_3->setDisabled(true);
		btnSkill_4->setDisabled(true);
	});



	connect(leUsername, &QLineEdit::returnPressed, btnLogin, &QPushButton::click);
	connect(lePassword, &QLineEdit::returnPressed, this, [this] {
		if (state == LOGIN)
		{
			btnLogin->click();
		}
		else if (state == CHANGE_PSW)
		{
			btnOK->click();
		}
	});
	connect(leNewPassword, &QLineEdit::returnPressed, btnOK, &QPushButton::click);





	client = new QTcpSocket(this);
	connect(client, &QTcpSocket::readyRead, this, &MainWindow::getServerMsg);

    // 初始化玩家的数据
	changingPokemonName = false;
	gettingDuelStatistic = false;
	battleStart = false;
	chooseBetIndex = 0;
	showPokemonDlg = false;
	pkmDlg[0] = NULL;
	pkmDlg[1] = NULL;
	pkmDlg[2] = NULL;

	changeState(START);


}

MainWindow::~MainWindow()
{
	delete client;
	delete ui;
}

void MainWindow::changeState(int aim)
{
    resize(width(), height());
    setStyleSheet(tr(".MainWindow{border-image:url(\"img/start3.jpg\");"
                     "background-attachment:fixed}"));//开始图形

    // 隐藏并重置所有的widget窗口
	lbStartTitle->hide();
	btnPlay->hide();
	btnExit->hide();
	lbLoginLabel->hide();
	leUsername->hide();
	lePassword->hide();
	btnLogin->hide();
	btnLogon->hide();
	btnBack->hide();
	btnLogout->hide();
	btnShowPokemonList->hide();
	btnLvUpBattle->hide();
	btnDuelBattle->hide();
	btnChangePassword->hide();
	btnDisplayAllPlayer->hide();
	btnOK->hide();
	leNewPassword->hide();
	lbWin->hide();
	lbTotal->hide();
	lbWinRate->hide();
	lbChooseEnemy->hide();
	lbEnemyLV->hide();
	sbEnemyLV->hide();
	btnEnemyRace0->hide();
	btnEnemyRace1->hide();
	btnEnemyRace2->hide();
	btnEnemyRace3->hide();
	lbP1->hide();
	lbP2->hide();
	btnSkill_1->hide();
	btnSkill_2->hide();
	btnSkill_3->hide();
	btnSkill_4->hide();
	pbP1HP->hide();
	pbP2HP->hide();
	lbP1SkillName->hide();
	lbP2SkillName->hide();
	lbBet[1]->hide();
	lbBet[2]->hide();
	lbBet[0]->hide();
	btnBet[1]->hide();
	btnBet[2]->hide();
	btnBet[0]->hide();
	lbPokemonMasterBadge->hide();
	lbPokemonNumBadge->hide();
	table->hide();
	table->clear();
	btnPlay->setDefault(false);
	btnBack->setDefault(false);
	btnLogin->setDefault(false);
	btnShowPokemonList->setDefault(false);
	btnOK->setDefault(false);

	disconnect(table, &QTableWidget::cellChanged, 0, 0);

	state = aim;

    //删去旧的布局，并新建一个新的布局
	delete ui->centralWidget->layout();
	layout = new QGridLayout(this);

	// show widgets in the certain state
	switch (state)
	{
	case START:
        lbStartTitle->showFullScreen();
		btnPlay->show();
		btnExit->show();
		setTabOrder(btnPlay, btnExit);
		setTabOrder(btnExit, btnPlay);
		layout->addWidget(lbStartTitle, 0, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop);
		layout->addWidget(btnPlay, 1, 0, 1, 1, Qt::AlignRight);
		layout->addWidget(btnExit, 2, 0, 1, 1, Qt::AlignRight);
		btnPlay->setDefault(true);
		break;
	case LOGIN:
		lbLoginLabel->show();
		leUsername->show();
		lePassword->clear();
		lePassword->show();
		btnLogin->show();
		btnLogon->show();
		btnBack->show();
		setTabOrder(leUsername, lePassword);
		setTabOrder(lePassword, btnLogin);
		setTabOrder(btnLogin, btnLogon);
		setTabOrder(btnLogon, btnBack);
		setTabOrder(btnBack, leUsername);
		layout->addWidget(lbLoginLabel, 0, 0, Qt::AlignHCenter | Qt::AlignTop);
		layout->addWidget(leUsername, 1, 0, Qt::AlignRight);
		layout->addWidget(lePassword, 2, 0, Qt::AlignRight);
		layout->addWidget(btnLogin, 3, 0, Qt::AlignRight);
		layout->addWidget(btnLogon, 4, 0, Qt::AlignRight);
		layout->addWidget(btnBack, 5, 0, Qt::AlignRight);
		btnLogin->setDefault(true);
		leUsername->setFocus();
		break;
	case MAIN:
        setStyleSheet(tr(".MainWindow{border-image:url(\"img/main.jpg\");"
                    "background-attachment:fixed}"));
		btnShowPokemonList->show();
		btnLvUpBattle->show();
		btnDuelBattle->show();
		btnDisplayAllPlayer->show();
		btnChangePassword->show();
		btnLogout->show();
		setTabOrder(btnShowPokemonList, btnLvUpBattle);
		setTabOrder(btnLvUpBattle, btnDuelBattle);
		setTabOrder(btnDuelBattle, btnDisplayAllPlayer);
		setTabOrder(btnDisplayAllPlayer, btnChangePassword);
		setTabOrder(btnChangePassword, btnLogout);
		setTabOrder(btnLogout, btnShowPokemonList);
		layout->addWidget(btnShowPokemonList, 0, 0, Qt::AlignRight);
		layout->addWidget(btnLvUpBattle, 1, 0, Qt::AlignRight);
		layout->addWidget(btnDuelBattle, 2, 0, Qt::AlignRight);
		layout->addWidget(btnDisplayAllPlayer, 3, 0, Qt::AlignRight);
		layout->addWidget(btnChangePassword, 4, 0, Qt::AlignRight);
		layout->addWidget(btnLogout, 5, 0, Qt::AlignRight);
		btnShowPokemonList->setDefault(true);
		break;
	case POKEMON_TABLE:
	case PLAYER_TABLE:
	case POKEMON_TABLE | LV_UP_BATTLE:
	case POKEMON_TABLE | DUEL_BATTLE:
		btnBack->show();
        btnBack->setFixedSize(400,200);
//        btnBack->set(Qt::AlignRight|Qt::AlignBottom);
		table->show();
		if (state == (POKEMON_TABLE | DUEL_BATTLE))
		{
			lbWin->show();
			lbTotal->show();
			lbWinRate->show();
			layout->addWidget(lbWin, 0, 0, 1, 1, Qt::AlignHCenter);
			layout->addWidget(lbTotal, 0, 1, 1, 1, Qt::AlignHCenter);
			layout->addWidget(lbWinRate, 0, 2, 1, 1, Qt::AlignHCenter);
			layout->addWidget(table, 1, 0, 1, 3);
			layout->addWidget(btnBack, 2, 0, 1, 3);
		}
		else if (state == POKEMON_TABLE)
		{
			lbPokemonMasterBadge->show();
			lbPokemonNumBadge->show();
			layout->addWidget(lbPokemonNumBadge, 0, 0, 1, 1, Qt::AlignHCenter);
			layout->addWidget(lbPokemonMasterBadge, 0, 1, 1, 1, Qt::AlignHCenter);
			layout->addWidget(table, 1, 0, 1, 2);
			layout->addWidget(btnBack, 2, 0, 1, 2);
		}
		else
		{
			layout->addWidget(table, 0, 0);
			layout->addWidget(btnBack, 1, 0);
		}
		btnBack->setDefault(true);
		break;
	case CHANGE_PSW:
		lePassword->clear();
		lePassword->show();
		leNewPassword->clear();
		leNewPassword->show();
		btnOK->show();
		btnBack->show();
		setTabOrder(lePassword, leNewPassword);
		setTabOrder(leNewPassword, btnOK);
		setTabOrder(btnOK, btnBack);
		setTabOrder(btnBack, lePassword);
		layout->addWidget(lePassword, 0, 0, 1, 1, Qt::AlignCenter);
		layout->addWidget(leNewPassword, 1, 0, 1, 1, Qt::AlignCenter);
		layout->addWidget(btnOK, 2, 0, 1, 1, Qt::AlignCenter);
		layout->addWidget(btnBack, 3, 0, 1, 1, Qt::AlignCenter);
		btnOK->setDefault(true);
		break;
	case CHOOSE_ENEMY | DUEL_BATTLE:
	case CHOOSE_ENEMY | LV_UP_BATTLE:
        setStyleSheet(tr(".MainWindow{background-image:url(\"img/vs.jpg\");"
                         "background-attachment:fixed}"));
		lbChooseEnemy->show();
		lbEnemyLV->show();
		sbEnemyLV->show();
		btnEnemyRace0->show();
		btnEnemyRace1->show();
		btnEnemyRace2->show();
		btnEnemyRace3->show();
		btnBack->show();
		if (state == (CHOOSE_ENEMY | DUEL_BATTLE))
		{
			sbEnemyLV->setValue(currentPokemonLV);
			sbEnemyLV->setDisabled(true);
		}
		else
		{
			sbEnemyLV->setValue(currentPokemonLV);
			sbEnemyLV->setDisabled(false);
		}
		layout->addWidget(lbChooseEnemy, 0, 0, 1, 2);
		layout->addWidget(lbEnemyLV, 0, 2, 1, 1, Qt::AlignRight);
		layout->addWidget(sbEnemyLV, 0, 3, 1, 1);
		layout->addWidget(btnEnemyRace0, 1, 0, 1, 2);
		layout->addWidget(btnEnemyRace1, 1, 2, 1, 2);
		layout->addWidget(btnEnemyRace2, 2, 0, 1, 2);
		layout->addWidget(btnEnemyRace3, 2, 2, 1, 2);
		layout->addWidget(btnBack, 3, 0, 1, 4);
		break;
	case LV_UP_BATTLE:
	case DUEL_BATTLE:
        setStyleSheet(tr(".MainWindow{background-image:url(\"img/vs.jpg\");"
                         "background-attachment:fixed}"));
		lbP1->show();
		lbP2->show();
		btnSkill_1->show();
		btnSkill_2->show();
		btnSkill_3->show();
		btnSkill_4->show();
		lbP1SkillName->show();
		lbP2SkillName->show();
		pbP1HP->show();
		pbP2HP->show();
		layout->addWidget(pbP1HP, 0, 0, 1, 2);
		layout->addWidget(lbP1SkillName, 1, 0, 1, 2, Qt::AlignCenter);
		layout->addWidget(pbP2HP, 0, 2, 1, 2);
		layout->addWidget(lbP2SkillName, 1, 2, 1, 2, Qt::AlignCenter);
		layout->addWidget(lbP1, 2, 0, 1, 2, Qt::AlignCenter);
		layout->addWidget(lbP2, 2, 2, 1, 2, Qt::AlignCenter);
		layout->addWidget(btnSkill_1, 3, 0, 1, 1);
		layout->addWidget(btnSkill_2, 3, 1, 1, 1);
		layout->addWidget(btnSkill_3, 3, 2, 1, 1);
		layout->addWidget(btnSkill_4, 3, 3, 1, 1);
		break;
	case CHOOSE_BET:
		setStyleSheet("");
		lbBet[1]->show();
		lbBet[2]->show();
		lbBet[0]->show();
		btnBet[1]->show();
		btnBet[2]->show();
		btnBet[0]->show();
		layout->addWidget(lbBet[0], 0, 0, 1, 1, Qt::AlignHCenter);
		layout->addWidget(lbBet[1], 0, 1, 1, 1, Qt::AlignHCenter);
		layout->addWidget(lbBet[2], 0, 2, 1, 1, Qt::AlignHCenter);
		layout->addWidget(btnBet[0], 1, 0, 1, 1);
		layout->addWidget(btnBet[1], 1, 1, 1, 1);
		layout->addWidget(btnBet[2], 1, 2, 1, 1);
		break;
	default:
		break;
	}
	ui->centralWidget->setLayout(layout);

	// special settings, must behind ui->centralWidget->setLayout()
	if (state == LOGIN)
	{
		leUsername->setFocus();
	}
	else if (state == MAIN)
	{
		btnShowPokemonList->setFocus();
	}
}

void MainWindow::login()
{
	if (!isValidUsername(leUsername->text()))
	{
		QMessageBox::warning(this, tr("错误"), tr("用户名不合法"));
		leUsername->clear();
		lePassword->clear();
		return;
	}
	if (!isValidPassword(lePassword->text()))
	{
		QMessageBox::warning(this, tr("错误"), tr("密码不合法"));
		lePassword->clear();
		return;
	}
    client->connectToHost(QHostAddress("127.0.0.1"), 7500);//与hub建立了连接
	QString msg = "login";
	msg += ' ';
	msg += leUsername->text();
	msg += ' ';
	msg += lePassword->text();

	btnLogin->setDisabled(true);

	if (client->write(msg.toLocal8Bit(), BUF_LENGTH) == -1)
	{
		// error
		QMessageBox::warning(this, tr("错误"), tr("服务器错误"));
		btnLogin->setDisabled(false);
	}
	lePassword->clear();
}

void MainWindow::getImproved(QLabel *lb)
{
	// move downward and move upward
	lb->move(lb->x(), lb->y() + 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() + 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() + 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() + 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() + 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() - 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() - 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() - 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() - 50);
	mySleep(50);
	lb->move(lb->x(), lb->y() - 50);
	mySleep(50);
}

void MainWindow::getDecreased(QLabel *lb)
{
	// blink several times
	lb->hide();
	mySleep(200);
	lb->show();
	mySleep(200);
	lb->hide();
	mySleep(200);
	lb->show();
	mySleep(200);
}

//非阻塞延时
void MainWindow::mySleep(int n)
{
	QTime t;
    t.start();  //开始计时
    while (t.elapsed() < n)//返回自上次调用start()或restart()以来经过的毫秒数。 //等待时间流逝n毫秒钟
        QCoreApplication::processEvents();//在等待的时间中，不停地处理事件，让程序保持响应
}

void MainWindow::getServerMsg()
{
    auto ret = client->read(BUF_LENGTH);//ret为client从server读过来的数据

	if (state == LOGIN)
		client->disconnectFromHost();

    QString msg = QString::fromLocal8Bit(ret);//msg即为从server读过来的数据

	if (changingPokemonName)
	{
		if (msg != "Accept.\n")
		{
			QMessageBox::warning(this, tr("错误"), tr("修改精灵名字失败"));
		}
		else
		{
			QMessageBox::information(this, tr("精灵修改名字"), tr("精灵名字已更新"));
		}

		changingPokemonName = false;
		return;
	}

    // 分析收到的msg
	switch (state)
	{
    case LOGIN://如果当前状态为LOGIN，正常情况下应该返回一个端口号
	{
		btnLogin->setDisabled(false);
		int port = msg.toInt();
		if (port == 0)
		{
			// login failed
			QMessageBox::warning(this, tr("错误"), msg);
		}
		else
		{
			// success
			username = leUsername->text();
			client->connectToHost(QHostAddress("127.0.0.1"), port);
			changeState(MAIN);
		}
		break;
	}
    case PLAYER_TABLE://对应的是Hub::getAllUser()函数
    {
        /**
         * msg format:
         * <playerID> <playername> <online:0|1> <win> <total>
         */
        auto players = msg.split('\n');//分割接受到的msg

        //! players[players.size() - 1] == ""

        table->setRowCount(players.size() - 2);
        table->setColumnCount(7); // id - username - online - win - total - win rate - viewPokemon
        table->setHorizontalHeaderLabels({tr("玩家ID"), tr("用户名"), tr("在线情况"), tr("决斗次数"), tr("决斗获胜次数"), tr("胜率"), tr("操作")});
        table->verticalHeader()->hide();//隐藏了水平表头
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

        int tableRowIndex = 0;
        for (int i = 0; i < players.size() - 1; ++i)
        {
            auto player = players[i];//第i条信息
            auto detail = player.split(' ');//提取第i条信息的各个信息
            // detail[0] is id, detail[1] is username
            if (detail[1] == username)
                continue; // 不显示玩家自己


            //设置一个表项id
            auto t = new QTableWidgetItem(detail[0]);
            t->setFlags(t->flags() ^ Qt::ItemIsEnabled);//设为不可被编辑
            if (detail[2] == '0')//不在线，设置为灰色
				t->setBackgroundColor(QColor("#eff0f1"));
            table->setItem(tableRowIndex, 0, t);


            //设置一个表项username
            t = new QTableWidgetItem(detail[1]);
            t->setFlags(t->flags() ^ Qt::ItemIsEnabled);//设为不可被编辑
            if (detail[2] == '0')//不在线，设置为灰色
				t->setBackgroundColor(QColor("#eff0f1"));
			table->setItem(tableRowIndex, 1, t);

            //设置一个表项online
            t = new QTableWidgetItem(detail[2] == '1' ? tr("在线") : tr("离线"));
            t->setFlags(t->flags() ^ Qt::ItemIsEnabled);//设为不可被编辑
            if (detail[2] == '0')//不在线，设置为灰色
				t->setBackgroundColor(QColor("#eff0f1"));
			table->setItem(tableRowIndex, 2, t);

            //设置一个表项win
			t = new QTableWidgetItem(detail[3]);
            t->setFlags(t->flags() ^ Qt::ItemIsEnabled);//设为不可被编辑
            if (detail[2] == '0')//不在线，设置为灰色
				t->setBackgroundColor(QColor("#eff0f1"));
			table->setItem(tableRowIndex, 3, t);

             //设置一个表项total
			t = new QTableWidgetItem(detail[4]);
            t->setFlags(t->flags() ^ Qt::ItemIsEnabled);//设为不可被编辑
            if (detail[2] == '0')//不在线，设置为灰色
				t->setBackgroundColor(QColor("#eff0f1"));
			table->setItem(tableRowIndex, 4, t);


            //设置一个表项win rate
			t = new QTableWidgetItem(detail[4] == "0" ? "-" : QString::number(detail[3].toDouble() / detail[4].toDouble()));
            t->setFlags(t->flags() ^ Qt::ItemIsEnabled);//设为不可被编辑
            if (detail[2] == '0')//不在线，设置为灰色
				t->setBackgroundColor(QColor("#eff0f1"));
			table->setItem(tableRowIndex, 5, t);

            //设置一个按钮查看pokemon
			auto btn = new QPushButton(tr("查看小精灵"), this);
			connect(btn, &QPushButton::clicked, this, [this, detail] {
				changeState(POKEMON_TABLE);
				currentPlayerID = detail[0].toInt();
				QString str = "getPokemonList ";
				str += detail[0];
                client->write(str.toStdString().c_str(), BUF_LENGTH);//发送给server
			});
			table->setCellWidget(tableRowIndex, 6, btn);
			++tableRowIndex;
		}
		break;
	}

	case POKEMON_TABLE:
	case POKEMON_TABLE | LV_UP_BATTLE:
	case POKEMON_TABLE | DUEL_BATTLE:
	{
        if (gettingDuelStatistic)  //对应的是Enpoint::getDuelStatistic()函数，返回的信息
        {                           //格式是获胜场数和总决斗场数
			gettingDuelStatistic = false;
            auto detail = msg.split(' ');//按空格先分割msg
			lbWin->setText(tr("获胜次数：") + detail[0]);
			lbTotal->setText(tr("决斗次数：") + detail[1]);
			lbWinRate->setText(tr("胜率：") + (detail[1] == '0' ? "-" : QString::number(detail[0].toDouble() / detail[1].toDouble())));
			break;
		}
		if (!showPokemonDlg) // msg is pokemon table
		{
			auto pokemons = msg.split('\n');

			table->setRowCount(pokemons.size() - 1);
			if (state == POKEMON_TABLE)
			{
				table->setColumnCount(5);
				table->setHorizontalHeaderLabels({tr("精灵ID"), tr("名字"), tr("种族"), tr("等级"), tr("操作")});
			}
			else
			{
				// battle table
				table->setColumnCount(6);
				table->setHorizontalHeaderLabels({tr("精灵ID"), tr("名字"), tr("种族"), tr("等级"), tr("查看"), tr("选中")});
			}
			table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
			table->verticalHeader()->hide();
			for (int i = 0; i < pokemons.size() - 1; ++i)
			{
				auto detail = pokemons[i].split(' ');
                // detail[0] is id, detail[1] is name, detail[2] is race，detail[3] is lv
				auto t = new QTableWidgetItem(detail[0]);
				t->setFlags(t->flags() ^ Qt::ItemIsEditable);
                table->setItem(i, 0, t);//设置位置

				t = new QTableWidgetItem(detail[1]);
				if (currentPlayerID == 0)
				{
					t->setToolTip(tr("双击以更改精灵名称"));
				}
				else
				{
					t->setFlags(t->flags() ^ Qt::ItemIsEditable);
				}
				table->setItem(i, 1, t);

//有所修改
                t = new QTableWidgetItem(detail[1]);
				t->setFlags(t->flags() ^ Qt::ItemIsEditable);
                if (detail[1] == "xiaohuohou")
                    t->setIcon(QIcon("img/xiaohuohou.png"));
                else if (detail[1] == "shumiaogui")
                    t->setIcon(QIcon("img/shumiaogui.png"));
                else if (detail[1] == "mobi")
                    t->setIcon(QIcon("img/mobi.jpg"));
                else if (detail[1] == "bokeer")
                    t->setIcon(QIcon("img/pidgey.png"));
				table->setItem(i, 2, t);


				t = new QTableWidgetItem(detail[3]);
				t->setFlags(t->flags() ^ Qt::ItemIsEditable);
				table->setItem(i, 3, t);

				auto btn = new QPushButton(tr("查看详情"), this);
				connect(btn, &QPushButton::clicked, this, [this, detail] {
					QString str = "getPokemon ";
					str += detail[0];
					client->write(str.toStdString().c_str(), BUF_LENGTH);
					showPokemonDlg = true;
				});
				table->setCellWidget(i, 4, btn);


				if (state != POKEMON_TABLE)
				{
                    // 说明它是一个对战表
                    auto btn = new QPushButton(tr("just choose you"), this);
					connect(btn, &QPushButton::clicked, this, [this, detail] {
						battlePokemonID = detail[0];
						currentPokemonLV = detail[3].toInt();
						changeState((state ^ POKEMON_TABLE) | CHOOSE_ENEMY);
					});
					table->setCellWidget(i, 5, btn);
				}
			}


			if (state == (DUEL_BATTLE | POKEMON_TABLE))
			{
				if (!gettingDuelStatistic)
				{
					gettingDuelStatistic = true;
					client->write("getDuelStatistic", BUF_LENGTH);
				}
			}

			connect(table, &QTableWidget::cellChanged, this, [this](int row, int column) {
				if ((state & POKEMON_TABLE) == POKEMON_TABLE && column == 1)
				{
					// pokemon name changed
					QString str = "pokemonChangeName ";
					str += table->item(row, 0)->text() + ' ' + table->item(row, 1)->text();
					client->write(str.toLocal8Bit(), BUF_LENGTH);
					changingPokemonName = true;
				}
			}); // must be connected after data input



            // 奖章系统
			int masterPokemon = 0; // number of lv-15 pokemon
			for (int i = 0; i < table->rowCount(); ++i)
			{
				if (table->item(i, 3)->text() == "15")
					++masterPokemon;
			}
			int pokemonNum = pokemons.size() - 1; // because pokemons[pokemons.size() - 1] == ""
			if (pokemonNum >= 20)
			{
                lbPokemonNumBadge->setPixmap(QPixmap("img/goldbadge.png").scaledToHeight(80));
				lbPokemonNumBadge->setToolTip(tr("金质精灵爱好者徽章，赐予精灵数量不少于20只的玩家"));
			}
			else if (pokemonNum >= 10)
			{
                lbPokemonNumBadge->setPixmap(QPixmap("img/silverbadge.png").scaledToHeight(80));
				lbPokemonNumBadge->setToolTip(tr("银质精灵爱好者徽章，赐予精灵数量不少于10只的玩家"));
			}
			else if (pokemonNum >= 5)
			{
                lbPokemonNumBadge->setPixmap(QPixmap("img/copperbadge.png").scaledToHeight(80));
				lbPokemonNumBadge->setToolTip(tr("铜质精灵爱好者徽章，赐予精灵数量不少于5只的玩家"));
			}
			else
			{
				lbPokemonNumBadge->setText(tr("精灵爱好者徽章：无"));
				lbPokemonNumBadge->setToolTip(tr("精灵数量大于5时可以获得精灵爱好者徽章"));
			}
			if (masterPokemon >= 5)
			{
                lbPokemonMasterBadge->setPixmap(QPixmap("img/goldenbadge2.png").scaledToHeight(80));
				lbPokemonMasterBadge->setToolTip(tr("金质精灵大师徽章，赐予15级精灵数量不少于5只的玩家"));
			}
			else if (masterPokemon >= 3)
			{
                lbPokemonMasterBadge->setPixmap(QPixmap("img/silverbadge2.png").scaledToHeight(80));
				lbPokemonMasterBadge->setToolTip(tr("银质精灵大师徽章，赐予15级精灵数量不少于3只的玩家"));
			}
			else if (masterPokemon >= 1)
			{
                lbPokemonMasterBadge->setPixmap(QPixmap("img/copperbadge2.png").scaledToHeight(80));
				lbPokemonMasterBadge->setToolTip(tr("铜质精灵大师徽章，赐予拥有15级精灵的玩家"));
			}
			else
			{
				lbPokemonMasterBadge->setText(tr("精灵大师徽章：无"));
				lbPokemonMasterBadge->setToolTip(tr("拥有15级精灵可以获得精灵大师徽章"));
			}
		}
		else // msg is pokemon detail
		{
			auto dlg = new PokemonDlg(msg, currentPlayerID == 0, this);
			connect(dlg, &PokemonDlg::pokemonChangeName, this, [this](const QString &pokemonID, const QString &newName) {
				for (int i = 0; i < table->rowCount(); ++i)
				{
					if (table->item(i, 0)->text() == pokemonID)
					{
						table->item(i, 1)->setText(newName); // will call table::cellChanged
					}
				}
			});
			showPokemonDlg = false;
		}
		break;
	}
	case CHANGE_PSW:
		if (msg == "Accept.\n")
		{
			QMessageBox::information(this, tr("密码重置"), tr("密码重置成功"));
			changeState(MAIN);
		}
		else
		{
			QMessageBox::warning(this, tr("错误"), msg);
		}
		break;
	case DUEL_BATTLE:
	case LV_UP_BATTLE:
	{
		if (battleStart)
		{
            //初始化4个技能的按钮都不能按动
			btnSkill_1->setDisabled(true);
			btnSkill_2->setDisabled(true);
			btnSkill_3->setDisabled(true);
			btnSkill_4->setDisabled(true);


            // get battle config message

            auto ps = msg.split('\n');
			auto detail = ps[0].split(' ');
            if (detail[0] == "xiaohuohou")
			{
                lbP1->setPixmap(QPixmap("img/xiaohuohou.png").scaledToHeight(500));
			}
            else if (detail[0] == "shumiaogui")
			{
                lbP1->setPixmap(QPixmap("img/shumiaogui.png").scaledToHeight(500));
			}
            else if (detail[0] == "mobi")
			{
                lbP1->setPixmap(QPixmap("img/mobi.jpg").scaledToHeight(500));
			}
            else if (detail[0] == "bokeer")
			{
                lbP1->setPixmap(QPixmap("img/pidgey.png").scaledToHeight(500));
			}
			pbP1HP->setMaximum(detail[1].toInt());
			pbP1HP->setValue(detail[1].toInt());
			btnSkill_1->setText(detail[2]);
			if (currentPokemonLV < 5)
			{
                btnSkill_2->setIcon(QIcon("img/lock.png"));
				btnSkill_2->setText(tr("等级5解锁"));
			}
			else
			{
                btnSkill_2->setText(detail[3]);
				btnSkill_2->setIcon(QIcon());
			}
			if (currentPokemonLV < 10)
			{
                btnSkill_3->setIcon(QIcon("img/lock.png"));
				btnSkill_3->setText(tr("等级10解锁"));
			}
			else
			{
				btnSkill_3->setIcon(QIcon());
                btnSkill_3->setText(detail[4]);
			}
			if (currentPokemonLV < 15)
			{
                btnSkill_4->setIcon(QIcon("img/lock.png"));
				btnSkill_4->setText(tr("等级15解锁"));
			}
			else
			{
				btnSkill_4->setIcon(QIcon());
                btnSkill_4->setText(detail[5]);
			}

            currentPokemonLV = detail[6].toInt();


			detail = ps[1].split(' ');
            if (detail[0] == "xiaohuohou")
			{
                lbP2->setPixmap(QPixmap("img/xiaohuohou.png").scaledToHeight(500));
			}
            else if (detail[0] == "shumiaogui")
			{
                lbP2->setPixmap(QPixmap("img/shumiaogui.png").scaledToHeight(500));
			}
            else if (detail[0] == "mobi")
			{
                lbP2->setPixmap(QPixmap("img/mobi.jpg").scaledToHeight(500));
			}
            else if (detail[0] == "bokeer")
			{
                lbP2->setPixmap(QPixmap("img/pidgey.png").scaledToHeight(500));
			}
			pbP2HP->setMaximum(detail[1].toInt());
			pbP2HP->setValue(detail[1].toInt());
			battleStart = false;
			client->write("done", BUF_LENGTH);
			break;
		}
        if (msg == "turn")  //玩家回复"turn",client释放技能的按钮变为可用
		{
			btnSkill_1->setDisabled(false);
            if (currentPokemonLV >= 5)
				btnSkill_2->setDisabled(false);
            if (currentPokemonLV >= 10)
				btnSkill_3->setDisabled(false);
            if (currentPokemonLV >= 15)
				btnSkill_4->setDisabled(false);
			break;
		}
		auto detail = msg.split(' ');



        //战斗时信息格式:<玩家回合0|1><技能名><闪避0|1><defenderHP0|1><defenderAtk:0|1|2><defenderdef0|1|2><dspeed><attackerHp0|1|2>
        //<aAtk><aDef><aspeed>
        //0表示下降，1表示不变，2表示上升
        if (detail[0] == '1')//玩家回合
		{
            lbP1SkillName->setText(detail[1]);//使用了xxx技能
			mySleep(500);
			if (detail[2] == '1')
			{
                // 闪避!
				lbP2SkillName->setText(tr("闪避！"));
				mySleep(500);
				lbP2SkillName->clear();
				lbP2SkillName->clear();
				client->write("done", BUF_LENGTH);
				break;
			}
            // 没有闪避
			if (pbP2HP->value() > detail[3].toInt())
			{
				// hp decreased
				lbP2SkillName->setText(QString::number(detail[3].toInt() - pbP2HP->value()));
				getDecreased(lbP2);
				while (pbP2HP->value() > detail[3].toInt())
				{
					mySleep(10);
					pbP2HP->setValue(pbP2HP->value() - 1);
				}
				lbP2SkillName->clear();
			}
			else if (pbP2HP->value() < detail[3].toInt())
			{
				// hp increased
				lbP2SkillName->setText(QString("+") + QString::number(detail[3].toInt() - pbP2HP->value()));
				getImproved(lbP2);
				while (pbP2HP->value() < detail[3].toInt())
				{
					mySleep(10);
					pbP2HP->setValue(pbP2HP->value() + 1);
				}
				lbP2SkillName->clear();
			}

            //detail[4]:defenderAtk
			if (detail[4] == '0')
			{
				lbP2SkillName->setText(tr("攻击下降"));
				getDecreased(lbP2);
				lbP2SkillName->clear();
			}
			else if (detail[4] == '2')
			{
				lbP2SkillName->setText(tr("攻击上升"));
				getImproved(lbP2);
				lbP2SkillName->clear();
			}

            //detail[5]:defenderDef
			if (detail[5] == '0')
			{
				lbP2SkillName->setText(tr("防御下降"));
				getDecreased(lbP2);
				lbP2SkillName->clear();
			}
			else if (detail[5] == '2')
			{
				lbP2SkillName->setText(tr("防御上升"));
				getImproved(lbP2);
				lbP2SkillName->clear();
			}

            //detail[6]:defenderspeed
			if (detail[6] == '0')
			{
				lbP2SkillName->setText(tr("速度下降"));
				getDecreased(lbP2);
				lbP2SkillName->clear();
			}
			else if (detail[6] == '2')
			{
				lbP2SkillName->setText(tr("速度上升"));
				getImproved(lbP2);
				lbP2SkillName->clear();
			}

            if (pbP1HP->value() > detail[7].toInt())
			{
				// hp decreased
                lbP1SkillName->setText(QString::number(detail[7].toInt() - pbP1HP->value()));
				getDecreased(lbP1);
                while (pbP1HP->value() > detail[7].toInt())
				{
					mySleep(10);
					pbP1HP->setValue(pbP1HP->value() - 1);
				}
				lbP1SkillName->clear();
			}
            else if (pbP1HP->value() < detail[7].toInt())
			{
				// hp increased
                lbP1SkillName->setText(QString("+") + QString::number(detail[7].toInt() - pbP1HP->value()));
				getImproved(lbP1);
                while (pbP1HP->value() < detail[7].toInt())
				{
					mySleep(10);
					pbP1HP->setValue(pbP1HP->value() + 1);
				}
				lbP1SkillName->clear();
			}


            if (detail[8] == '0')
			{
				lbP1SkillName->setText(tr("攻击下降"));
				getDecreased(lbP1);
				lbP1SkillName->clear();
			}
            else if (detail[8] == '2')
			{
				lbP1SkillName->setText(tr("攻击上升"));
				getImproved(lbP1);
				lbP1SkillName->clear();
			}
            if (detail[9] == '0')
			{
				lbP1SkillName->setText(tr("防御下降"));
				getDecreased(lbP1);
				lbP1SkillName->clear();
			}
            else if (detail[9] == '2')
			{
				lbP1SkillName->setText(tr("防御上升"));
				getImproved(lbP1);
				lbP1SkillName->clear();
			}
            if (detail[10] == '0')
			{
				lbP1SkillName->setText(tr("速度下降"));
				getDecreased(lbP1);
				lbP1SkillName->clear();
			}
            else if (detail[10] == '2')
			{
				lbP1SkillName->setText(tr("速度上升"));
				getImproved(lbP1);
				lbP1SkillName->clear();
			}
			auto skillName = btnSkill_2->text().split(' ')[0];
			if (currentPokemonLV < 5)
				btnSkill_2->setText(tr("等级5解锁"));
			else
                btnSkill_2->setText(skillName);
			skillName = btnSkill_3->text().split(' ')[0];
			if (currentPokemonLV < 10)
				btnSkill_3->setText(tr("等级10解锁"));
			else
                btnSkill_3->setText(skillName);
			skillName = btnSkill_4->text().split(' ')[0];
			if (currentPokemonLV < 15)
				btnSkill_4->setText(tr("等级15解锁"));
			else
                btnSkill_4->setText(skillName);
			lbP1SkillName->clear();
		}
		else
		{
			// detail[0] == '0', p2's turn
			lbP2SkillName->setText(detail[1]);
			mySleep(500);
			if (detail[2] == '1')
			{
				// dodge!
				lbP1SkillName->setText(tr("闪避！"));
				mySleep(500);
				lbP1SkillName->clear();
				lbP2SkillName->clear();
				client->write("done", BUF_LENGTH);
				break;
			}


			// not dodge
			if (pbP1HP->value() > detail[3].toInt())
			{
				// hp decreased
				lbP1SkillName->setText(QString::number(detail[3].toInt() - pbP1HP->value()));
				getDecreased(lbP1);
				while (pbP1HP->value() > detail[3].toInt())
				{
					mySleep(10);
					pbP1HP->setValue(pbP1HP->value() - 1);
				}
				lbP1SkillName->clear();
			}
			else if (pbP1HP->value() < detail[3].toInt())
			{
				// hp increased
				lbP1SkillName->setText(QString("+") + QString::number(detail[3].toInt() - pbP1HP->value()));
				getImproved(lbP1);
				while (pbP1HP->value() < detail[3].toInt())
				{
					mySleep(10);
					pbP1HP->setValue(pbP1HP->value() + 1);
				}
				lbP1SkillName->clear();
			}


			if (detail[4] == '0')
			{
				lbP1SkillName->setText(tr("攻击下降"));
				getDecreased(lbP1);
				lbP1SkillName->clear();
			}
			else if (detail[4] == '2')
			{
				lbP1SkillName->setText(tr("攻击上升"));
				getImproved(lbP1);
				lbP1SkillName->clear();
			}


			if (detail[5] == '0')
			{
				lbP1SkillName->setText(tr("防御下降"));
				getDecreased(lbP1);
				lbP1SkillName->clear();
			}
			else if (detail[5] == '2')
			{
				lbP1SkillName->setText(tr("防御上升"));
				getImproved(lbP1);
				lbP1SkillName->clear();
			}


			if (detail[6] == '0')
			{
				lbP1SkillName->setText(tr("速度下降"));
				getDecreased(lbP1);
				lbP1SkillName->clear();
			}
			else if (detail[6] == '2')
			{
				lbP1SkillName->setText(tr("速度上升"));
				getImproved(lbP1);
				lbP1SkillName->clear();
			}


            if (pbP2HP->value() > detail[7].toInt())
			{
				// hp decreased
                lbP2SkillName->setText(QString::number(detail[7].toInt() - pbP2HP->value()));
				getDecreased(lbP2);
                while (pbP2HP->value() > detail[7].toInt())
				{
					mySleep(10);
					pbP2HP->setValue(pbP2HP->value() - 1);
				}
				lbP2SkillName->clear();
			}
            else if (pbP2HP->value() < detail[7].toInt())
			{
				// hp increased
                lbP2SkillName->setText(QString("+") + QString::number(detail[7].toInt() - pbP2HP->value()));
				getImproved(lbP2);
                while (pbP2HP->value() < detail[7].toInt())
				{
					mySleep(10);
					pbP2HP->setValue(pbP2HP->value() + 1);
				}
				lbP2SkillName->clear();
			}

            if (detail[8] == '0')
			{
				lbP2SkillName->setText(tr("攻击下降"));
				getDecreased(lbP2);
				lbP2SkillName->clear();
			}
            else if (detail[8] == '2')
			{
				lbP2SkillName->setText(tr("攻击上升"));
				getImproved(lbP2);
				lbP2SkillName->clear();
			}
            if (detail[9] == '0')
			{
				lbP2SkillName->setText(tr("防御下降"));
				getDecreased(lbP2);
				lbP2SkillName->clear();
			}
            else if (detail[9] == '2')
			{
				lbP2SkillName->setText(tr("防御上升"));
				getImproved(lbP2);
				lbP2SkillName->clear();
			}
            if (detail[10] == '0')
			{
				lbP2SkillName->setText(tr("速度下降"));
				getDecreased(lbP2);
				lbP2SkillName->clear();
			}
            else if (detail[10] == '2')
			{
				lbP2SkillName->setText(tr("速度上升"));
				getImproved(lbP2);
				lbP2SkillName->clear();
			}
            auto skillName = btnSkill_2->text();
			if (currentPokemonLV < 5)
				btnSkill_2->setText(tr("等级5解锁"));
			else
                btnSkill_2->setText(skillName);
            skillName = btnSkill_3->text();
			if (currentPokemonLV < 10)
				btnSkill_3->setText(tr("等级10解锁"));
			else
                btnSkill_3->setText(skillName);
            skillName = btnSkill_4->text();
			if (currentPokemonLV < 15)
				btnSkill_4->setText(tr("等级15解锁"));
			else
                btnSkill_4->setText(skillName);
			lbP2SkillName->clear();
		}

		// judge result
		if (pbP2HP->value() == 0)
		{
			if (state == LV_UP_BATTLE)
				QMessageBox::information(this, tr("恭喜"), tr("你赢得了战斗"));
			else
				QMessageBox::information(this, tr("恭喜"), tr("你赢得了决斗，获得了敌对精灵"));
			changeState(MAIN);
			break;
		}
		else if (pbP1HP->value() == 0)
		{
			QMessageBox::information(this, tr("抱歉"), tr("您战败了"));
			if (state == DUEL_BATTLE)
			{
				changeState(CHOOSE_BET);
				chooseBetIndex = 0;
				client->write("chooseBet", BUF_LENGTH);
				break;
			}
			changeState(MAIN);
			break;
		}
		client->write("done", BUF_LENGTH);

		break;
	}
	case CHOOSE_BET:
	{
		++chooseBetIndex;
		pkmDlg[chooseBetIndex - 1] = new PokemonDlg(msg, false, this);
		if (chooseBetIndex == 1)
			pkmDlg[chooseBetIndex - 1]->move(pkmDlg[chooseBetIndex - 1]->x() - pkmDlg[chooseBetIndex - 1]->width(), pkmDlg[chooseBetIndex - 1]->y());
		else if (chooseBetIndex == 3)
			pkmDlg[chooseBetIndex - 1]->move(pkmDlg[chooseBetIndex - 1]->x() + pkmDlg[chooseBetIndex - 1]->width(), pkmDlg[chooseBetIndex - 1]->y());
    //	lbBet[chooseBetIndex - 1]->setPixmap(QPixmap(*pkmDlg[chooseBetIndex - 1]->getPixmap()));

		btnBet[chooseBetIndex - 1]->setText(tr("我选择") + pkmDlg[chooseBetIndex - 1]->getName());
		auto id = pkmDlg[chooseBetIndex - 1]->getID();
		connect(btnBet[chooseBetIndex - 1], &QPushButton::clicked, [this, id] {
			QString str = "discard ";
			str += id;
			client->write(str.toLocal8Bit(), BUF_LENGTH);
			for (int i = 0; i < 3; ++i)
			{
				if (pkmDlg[i] != NULL)
//					pkmDlg[i]->deleteLater();
					pkmDlg[i]->close();
			}
			changeState(MAIN);
		});
		connect(pkmDlg[chooseBetIndex - 1], &PokemonDlg::destroyed, [this] {
			pkmDlg[chooseBetIndex - 1] = NULL;
		});
		if (chooseBetIndex < 4)
			client->write("done", BUF_LENGTH);
		break;
	}
	default:
		break;
	}
}
