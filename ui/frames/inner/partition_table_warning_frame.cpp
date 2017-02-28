// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "partition_table_warning_frame.h"

#include <QDebug>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>

#include "base/file_util.h"
#include "ui/frames/consts.h"
#include "ui/widgets/comment_label.h"
#include "ui/widgets/expanded_nav_button.h"
#include "ui/widgets/title_label.h"

namespace installer {

PartitionTableWarningFrame::PartitionTableWarningFrame(QWidget* parent)
    : QFrame(parent),
      device_path_() {
  this->setObjectName("partition_table_warning_frame");

  this->initUI();
  this->initConnections();
}

QString PartitionTableWarningFrame::devicePath() const {
  return device_path_;
}

void PartitionTableWarningFrame::setDevicePath(const QString& device_path) {
  device_path_ = device_path;
}

void PartitionTableWarningFrame::changeEvent(QEvent* event) {
  if (event->type() == QEvent::LanguageChange) {
    title_label_->setText(tr("Warning"));
    comment_label_->setText(
        tr("It is detected that your motherboard boot loader "
           "is EFI and the disk format is MBR, thus the system can not be "
           "installed directly; to continue, please select one of "
           "the solutions below."));
    list_title1_->setText(tr("A. Disable UEFI"));
    list_item1_->setText(
        tr("1.Reboot, enter BIOS, and disable UEFI\n"
           "2.Exit BIOS, and enter deepin installation"));
    list_title2_->setText(tr("B.Format the disk"));
    list_item2_->setText(
        tr("1.Please make backup for all your data to avoid data loss\n"
           "2.Please double check if you have made backup for all the data, "
           "and reboot to enter this interface, Please double check if "
           "you have"));
    list_title3_->setText(tr("Continue"));
    list_item3_->setText(
        tr("1.Please make sure you have made backup for all the data, then "
           "continue\n"
           "2.Continue installation will format your disk"));
    reject_button_->setText(tr("Reboot"));
    accept_button_->setText(tr("Continue"));
    cancel_button_->setText(tr("Back"));
  } else {
    QFrame::changeEvent(event);
  }
}

void PartitionTableWarningFrame::initConnections() {
  connect(reject_button_, &QPushButton::clicked,
          this, &PartitionTableWarningFrame::reboot);
  connect(accept_button_, &QPushButton::clicked,
          this, &PartitionTableWarningFrame::onConfirmButtonClicked);
  connect(cancel_button_, &QPushButton::clicked,
          this, &PartitionTableWarningFrame::canceled);
}

void PartitionTableWarningFrame::initUI() {
  QLabel* warning_label = new QLabel();
  QPixmap warning_pixmap(":/images/warning.png");
  warning_label->setPixmap(warning_pixmap);
  title_label_ = new TitleLabel(tr("Warning"));
  QHBoxLayout* title_layout = new QHBoxLayout();
  title_layout->setContentsMargins(0, 0, 0, 0);
  title_layout->setSpacing(0);
  title_layout->addStretch();
  title_layout->addWidget(warning_label);
  title_layout->addSpacing(8);
  title_layout->addWidget(title_label_);
  title_layout->addStretch();

  comment_label_ = new CommentLabel(
      tr("It is detected that your motherboard boot loader "
         "is EFI and the disk format is MBR, thus the system can not be "
         "installed directly; to continue, please select one of "
         "the solutions below."));
  QHBoxLayout* comment_layout = new QHBoxLayout();
  comment_layout->setContentsMargins(0, 0, 0, 0);
  comment_layout->setSpacing(0);
  comment_layout->addWidget(comment_label_);

  list_title1_ = new QLabel(tr("A. Disable UEFI"), this);
  list_title1_->setObjectName("list_title1");
  list_item1_ = new QLabel(
      tr("1.Reboot, enter BIOS, and disable UEFI\n"
         "2.Exit BIOS, and enter deepin installation"), this);
  list_item1_->setObjectName("list_item1");
  list_item1_->setWordWrap(true);
  list_title2_ = new QLabel(tr("B.Format the disk"), this);
  list_title2_->setObjectName("list_title2");
  list_item2_ = new QLabel(
      tr("1.Please make backup for all your data to avoid data loss\n"
         "2.Please double check if you have made backup for all the data, "
         "and reboot to enter this interface, Please double check if you have"),
      this);
  list_item2_->setObjectName("list_item2");
  list_item2_->setWordWrap(true);
  QVBoxLayout* left_frame_layout = new QVBoxLayout();
  left_frame_layout->setContentsMargins(20, 20, 20, 20);
  left_frame_layout->setSpacing(0);
  left_frame_layout->addWidget(list_title1_);
  left_frame_layout->addWidget(list_item1_);
  left_frame_layout->addStretch();
  left_frame_layout->addSpacing(5);
  left_frame_layout->addWidget(list_title2_);
  left_frame_layout->addWidget(list_item2_);
  left_frame_layout->addStretch();
  QFrame* left_frame = new QFrame();
  left_frame->setObjectName("left_frame");
  left_frame->setFixedWidth(480);
  left_frame->setLayout(left_frame_layout);

  list_title3_ = new QLabel(tr("Continue"), this);
  list_title3_->setObjectName("list_title3");
  list_item3_ = new QLabel(
      tr("1.Please make sure you have made backup for all the data, then "
         "continue\n"
         "2.Continue installation will format your disk"),
      this);
  list_item3_->setObjectName("list_item3");
  list_item3_->setWordWrap(true);
  QVBoxLayout* right_frame_layout = new QVBoxLayout();
  right_frame_layout->setContentsMargins(20, 20, 20, 20);
  right_frame_layout->setSpacing(0);
  right_frame_layout->addWidget(list_title3_);
  right_frame_layout->addWidget(list_item3_);
  right_frame_layout->addStretch();
  QFrame* right_frame = new QFrame();
  right_frame->setObjectName("right_frame");
  right_frame->setFixedWidth(480);
  right_frame->setLayout(right_frame_layout);

  reject_button_ = new ExpandedNavButton(tr("Reboot"));
  accept_button_ = new ExpandedNavButton(tr("Continue"));
  cancel_button_ = new NavButton(tr("Back"));

  QGridLayout* content_layout = new QGridLayout();
  content_layout->setContentsMargins(0, 0, 0, 0);
  content_layout->setAlignment(Qt::AlignCenter);
  content_layout->setVerticalSpacing(kMainLayoutSpacing);
  content_layout->setHorizontalSpacing(40);
  content_layout->setColumnMinimumWidth(0, 480);
  content_layout->setColumnMinimumWidth(1, 480);
  content_layout->addWidget(left_frame, 0, 0);
  content_layout->addWidget(right_frame, 0, 1);
  content_layout->addWidget(reject_button_, 1, 0);
  content_layout->addWidget(accept_button_, 1, 1);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(kMainLayoutSpacing);
  layout->addStretch();
  layout->addLayout(title_layout);
  layout->addLayout(comment_layout);
  layout->addStretch();
  layout->addLayout(content_layout);
  layout->addStretch();
  layout->addWidget(cancel_button_, 0, Qt::AlignHCenter);

  this->setLayout(layout);
  this->setContentsMargins(0, 0, 0, 0);
  this->setStyleSheet(ReadFile(":/styles/partition_table_warning_frame.css"));
}

void PartitionTableWarningFrame::onConfirmButtonClicked() {
  emit this->confirmed(device_path_);
}

}  // namespace installer