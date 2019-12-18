#pragma once

#include <QWidget>
#include <qtreewidget.h>
namespace Ui { class ToolWidget; };

class ToolWidget : public QWidget
{
	Q_OBJECT

public:
	ToolWidget(QWidget *parent = Q_NULLPTR);
	~ToolWidget();

private:
	Ui::ToolWidget *ui;
	QTreeWidget* toolTree;
	void initToolTree();
public slots:
	void on_item_clicked(QTreeWidgetItem* item);
};
