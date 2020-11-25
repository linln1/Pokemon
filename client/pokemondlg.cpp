#include "pokemondlg.h"
#include "ui_pokemondlg.h"
#include <QVBoxLayout>
#include<QTableWidget>
#include<QHeaderView>
#include <QString>
#include<qDebug>


PokemonDlg::PokemonDlg(const QString &detail, bool myPokemonTable, QWidget *parent) : QDialog(parent),
																																											ui(new Ui::PokemonDlg)
{
	ui->setupUi(this);

	// ui
	setWindowTitle(tr("精灵详情"));
	table = new QTableWidget(this);
    table->setFixedSize(800,900);
    table->setColumnCount(1);
    table->setRowCount(9);
	table->setVerticalHeaderLabels({tr("精灵ID"), tr("名字"), tr("种族"), tr("攻击力"), tr("防御力"), tr("生命值"), tr("速度"), tr("等级"), tr("经验")});
	table->horizontalHeader()->hide();
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


	// data
	auto details = detail.split(' ');
    //Debug Test
    qDebug() << details;


	for (int i = 0; i < 9; ++i)
	{
		auto t = new QTableWidgetItem(details[i]);

		if (i != 1)
		{
			// only pokemon name can be changed
			t->setFlags(t->flags() ^ Qt::ItemIsEnabled);
			t->setBackgroundColor(QColor("#eff0f1"));
		}
		else
		{
			if (myPokemonTable)
			{
				t->setToolTip(tr("双击以更改精灵名称"));
			}
			else
			{
				// can not change other players' pokemons' name
				t->setFlags(t->flags() ^ Qt::ItemIsEditable);
				t->setBackgroundColor(QColor("#eff0f1"));
			}
		}
		table->setItem(i, 0, t);
	}
	connect(table, &QTableWidget::cellChanged, this, [this, detail] { emit pokemonChangeName(detail[0], table->item(1, 0)->text()); });

	// img
	lbImg = new QLabel(this); // name
    QPixmap *p1 = new QPixmap("img/shumiaogui.png");

    QPixmap *p2 = new QPixmap("img/xiaohuohou.png");

    QPixmap *p3 = new QPixmap("img/mobi.jpg");

    QPixmap *p4 = new QPixmap("img/pidgey.png");

    p1->scaled(lbImg->size(),Qt::KeepAspectRatio);
    p2->scaled(lbImg->size(),Qt::KeepAspectRatio);
    p3->scaled(lbImg->size(),Qt::KeepAspectRatio);
    p4->scaled(lbImg->size(),Qt::KeepAspectRatio);
    lbImg->setScaledContents(true);

    // if (details[2] == "shumiaogui")
    if (details[2] == "shumiaogui")
	{
        lbImg->setPixmap(*p1);
        setWindowIcon(QIcon("img/shumiaogui.png"));
	}
    // else if (details[2] == "img/xiaohuohou")
    else if (details[2] == "xiaohuohou")
	{
        lbImg->setPixmap(*p2);
        setWindowIcon(QIcon("img/xiaohuohou.png"));
	}
    // else if (details[2] == "mobi")
    else if (details[2] == "mobi")
	{
        lbImg->setPixmap(*p3);
        setWindowIcon(QIcon("img/mobi.jpg"));
	}
    //else if (details[2] == "bokeer")
    else if (details[2] == "bokeer")
	{

        lbImg->setPixmap(*p4);
        setWindowIcon(QIcon("img/pidgey.png"));
	}

	// layout
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(lbImg, 0, Qt::AlignCenter);
	layout->addWidget(table, 0, Qt::AlignHCenter);
	setLayout(layout);
	setFixedSize(340, 670); // can not resize window

	// memory
	setAttribute(Qt::WA_DeleteOnClose);

	// setup ui after data filling
	setWindowFlags(windowFlags() ^ Qt::WindowContextHelpButtonHint);
	show();
}

PokemonDlg::~PokemonDlg()
{
	delete ui;
}
