#ifndef QUEUE_ITEM_H
#define QUEUE_ITEM_H

class QueueItem {
public:
    int appointmentId;
    int position;

    QueueItem() {}
    QueueItem(int appointmentId, int position);
};

#endif