#include <gtest/gtest.h>
#include "rtmc_path.h"

TEST(PathQueueTests, Queue_Typical) {
    // create queue
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    EXPECT_FALSE(queue.head);
    EXPECT_FALSE(queue.tail);

    // create paths
    rtmc_path_t path1;
    rtmc_path_t path2;
    rtmc_path_t path3;
    path1.feed_rate = 100;
    path2.feed_rate = 200;
    path3.feed_rate = 300;

    // add paths to queue
    rtmc_path_enqueue(&queue, path1);
    rtmc_path_enqueue(&queue, path2);
    rtmc_path_enqueue(&queue, path3);
    EXPECT_TRUE(queue.head);
    EXPECT_TRUE(queue.tail);

    // remove paths from queue and check result
    rtmc_path_t new_path;
    new_path = rtmc_path_dequeue(&queue);
    EXPECT_DOUBLE_EQ(new_path.feed_rate, path1.feed_rate);
    new_path = rtmc_path_dequeue(&queue);
    EXPECT_DOUBLE_EQ(new_path.feed_rate, path2.feed_rate);
    new_path = rtmc_path_dequeue(&queue);
    EXPECT_DOUBLE_EQ(new_path.feed_rate, path3.feed_rate);

    // check that queue is now empty
    EXPECT_FALSE(queue.head);
    EXPECT_FALSE(queue.tail);
}

TEST(PathQueueTests, Queue_Underflow) {
    // initialize an empty queue
    rtmc_path_queue_t queue = rtmc_create_path_queue();
    EXPECT_NO_THROW(rtmc_path_dequeue(&queue));

    EXPECT_FALSE(queue.head);
    EXPECT_FALSE(queue.tail);
}

TEST(PathQueueTests, Queue_Peek) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();

    // create path
    rtmc_path_t path;
    path.feed_rate = 100;

    rtmc_path_enqueue(&queue, path);

    rtmc_path_t new_path = rtmc_path_queue_peek(&queue);
    EXPECT_DOUBLE_EQ(new_path.feed_rate, path.feed_rate);

    // check that queue isn't empty
    EXPECT_TRUE(queue.head);
    EXPECT_TRUE(queue.tail);

}

TEST(PathQueueTests, Queue_Flush) {
    rtmc_path_queue_t queue = rtmc_create_path_queue();

    // create path
    rtmc_path_t path;
    path.feed_rate = 100;

    // add a few nodes to the queue
    rtmc_path_enqueue(&queue, path);
    rtmc_path_enqueue(&queue, path);
    rtmc_path_enqueue(&queue, path);

    // flush the queue and verify
    rtmc_flush_path_queue(&queue);
    EXPECT_FALSE(queue.head);
    EXPECT_FALSE(queue.tail);
}
