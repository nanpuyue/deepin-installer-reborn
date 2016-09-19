// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "ui/delegates/operation_resize.h"

namespace ui {

OperationResize::OperationResize(const partman::Partition& partition_orig,
                                 const partman::Partition& partition_new) {
  this->type = OperationType::Resize;
  this->partition_orig = partition_orig;
  this->partition_new = partition_new;
}

void OperationResize::applyToVisual(partman::PartitionList& partitions) const {
  Q_UNUSED(partitions);
}

}  // namespace ui