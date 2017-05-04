/*
 * Team 13-03 CS444 Oregon State Uiversity Spring 2017
 * Josh Asher, Joseph Struth, Bryan Liauw
 * elevator LOOP scheduler or Shortest Seek Time First (sstf)
 * sstf-iosched.c
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

struct sstf_data {
	struct list_head queue;
	sector_t position;
	unsigned char direction;
};

static void sstf_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int sstf_dispatch(struct request_queue *q, int force)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		return 1;
	}
	return 0;
}

static void sstf_add_request(struct request_queue *q, struct request *rq)
{
	/*Grrab the que that we are working*/
	struct sstf_data *nd = q->elevator->elevator_data;
	
	if(list_empty(&nd->que){
		list_add(&rq->quelist, &sd->que)
	}
	printk(KERN_INFO "SSTF - Que emtpy Directly Inserting");
	list_add_tail(&rq->queuelist, &nd->queue);
}

static struct request *
sstf_former_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
sstf_latter_request(struct request_queue *q, struct request *rq)
{
	struct sstf_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

/*Initialize all required data for sstf*/
static int sstf_init_queue(struct request_queue *q, struct elevator_type *e)
{
	struct sstf_data *nd;
	struct elevator_queue *eq;

	eq = elevator_alloc(q, e);
	if (!eq)
		return -ENOMEM;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;

	INIT_LIST_HEAD(&nd->queue);
	
	/*Set head position to zero to start, and the length of the request to 0 to start*/
	nd->position = 0;
	printk(KERN_INFO "LOOK Sceduler Initialized");
	
	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	return 0;
}

/*Take an item out of the que */
static void sstf_exit_queue(struct elevator_queue *e)
{
	struct sstf_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

/*This links our scheduler into the kernel with the required structures */
static struct elevator_type elevator_sstf = {
	.ops = {
		.elevator_merge_req_fn		= sstf_merged_requests,
		.elevator_dispatch_fn		= sstf_dispatch,
		.elevator_add_req_fn		= sstf_add_request,
		.elevator_former_req_fn		= sstf_former_request,
		.elevator_latter_req_fn		= sstf_latter_request,
		.elevator_init_fn		= sstf_init_queue,
		.elevator_exit_fn		= sstf_exit_queue,
	},
	.elevator_name = "sstf",
	.elevator_owner = THIS_MODULE,
};

/* Called on our initialization returns reference to our elevator structure */
static int __init sstf_init(void)
{
	return elv_register(&elevator_sstf);
}

/*
* Shut down sstf when it is no longer needed, unregister our io shceduler from the kernel
*/
static void __exit sstf_exit(void)
{
	elv_unregister(&elevator_sstf);
}

module_init(sstf_init);
module_exit(sstf_exit);


MODULE_AUTHOR("Jens Axboe, Josh Asher, Joseph Struth, Bryan Liauw");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LOOP IO scheduler");
