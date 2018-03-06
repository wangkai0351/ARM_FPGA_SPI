#ifndef _QUEUE_H
#define _QUEUE_H


extern Uint8   QueueCreate(DQ_Q *buf, DQ_M *dq, Uint16 size);
extern Uint8   MyQueueWrite(DQ_M *dq, struct pbuf *p, struct netif *netif);
Uint8   QueueWrite(DQ_M *dq, struct pbuf *p, struct netif *netif);
extern DQ_Q    *QueueRead(DQ_M *dq);
extern Uint16  QueueQuery(DQ_M *dq);

#endif
