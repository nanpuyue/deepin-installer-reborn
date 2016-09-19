// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "ui/frames/inner/advanced_partition_frame.h"

#include <QButtonGroup>
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include "ui/delegates/partition_delegate.h"
#include "ui/widgets/advanced_partition_item.h"
#include "ui/widgets/flat_button.h"

namespace ui {

AdvancedPartitionFrame::AdvancedPartitionFrame(
    PartitionDelegate* partition_delegate, QWidget* parent)
    : QFrame(parent),
      partition_delegate_(partition_delegate) {
  this->setObjectName(QStringLiteral("advanced_partition_frame"));

  this->initUI();
  this->initConnections();
}

void AdvancedPartitionFrame::initConnections() {
  connect(partition_delegate_, &PartitionDelegate::deviceRefreshed,
          this, &AdvancedPartitionFrame::onDeviceRefreshed);

  connect(bootloader_selection_button_, &QPushButton::clicked,
          this, &AdvancedPartitionFrame::requestSelectBootloaderFrame);

  connect(enable_editing_button_, &QPushButton::toggled,
          this, &AdvancedPartitionFrame::onEditButtonToggled);
}

void AdvancedPartitionFrame::initUI() {
  partition_layout_ = new QVBoxLayout();

  partition_button_group_ = new QButtonGroup(this);

  QFrame* wrapper = new QFrame();
  wrapper->setLayout(partition_layout_);
  QScrollArea* main_area = new QScrollArea();
  main_area->setWidget(wrapper);
//  main_area->resize(640, 480);
  partition_layout_->setSizeConstraint(QLayout::SetMinAndMaxSize);

  bootloader_selection_button_ = new FlatButton(tr("Select bootloader"));
  enable_editing_button_ = new FlatButton(tr("Edit"));
  enable_editing_button_->setCheckable(true);
  enable_editing_button_->setChecked(false);
  QHBoxLayout* bottom_layout = new QHBoxLayout();
  bottom_layout->addWidget(bootloader_selection_button_);
  bottom_layout->addStretch();
  bottom_layout->addWidget(enable_editing_button_);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->addWidget(main_area);
  layout->addLayout(bottom_layout);

  this->setLayout(layout);
}

void AdvancedPartitionFrame::drawDevices() {
  qDebug() << "AdvancedPartitionFrame::drawDevice()";
  // Clear children in button group.
  for (QAbstractButton* button : partition_button_group_->buttons()) {
    partition_button_group_->removeButton(button);
  }

  // Remove all widgets in partition layout.
  for (QLayoutItem* item = partition_layout_->takeAt(0); item != NULL;
       item = partition_layout_->takeAt(0)) {
    delete item->widget();
    delete item;
  }

  for (const partman::Device& device : partition_delegate_->devices()) {
    QLabel* model_label = new QLabel(device.model);
    partition_layout_->addWidget(model_label);
    qDebug() << "Add model:" << device.model;
    for (const partman::Partition& partition : device.partitions) {
      AdvancedPartitionItem* item = new AdvancedPartitionItem(partition);
      partition_layout_->addWidget(item);
      partition_button_group_->addButton(item);
      item->show();

      connect(enable_editing_button_, &QPushButton::toggled,
              item, &AdvancedPartitionItem::setEditable);

      connect(item, &AdvancedPartitionItem::editPartitionTriggered,
              this, &AdvancedPartitionFrame::requestEditPartitionFrame);
      connect(item, &AdvancedPartitionItem::newPartitionTriggered,
              this, &AdvancedPartitionFrame::requestNewPartitionFrame);

      connect(item, &AdvancedPartitionItem::deletePartitionTriggered,
              partition_delegate_, &PartitionDelegate::deletePartition);
    }
  }
}

void AdvancedPartitionFrame::onDeviceRefreshed() {
  this->drawDevices();
}

void AdvancedPartitionFrame::onEditButtonToggled(bool toggle) {
  if (toggle) {
    enable_editing_button_->setText(tr("Done"));
  } else {
    enable_editing_button_->setText(tr("Edit"));
  }
}

}  // namespace ui