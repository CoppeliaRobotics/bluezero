#include <b0/node.h>
#include <b0/publisher.h>
#include "publisher_subscriber.pb.h"

#include <iostream>
#include <boost/lexical_cast.hpp>

/*! \example publisher_node_object.cpp
 * This is an example of creating a node by subclassing b0::Node.
 * Useful for overriding some node's behavior.
 */

//! \cond HIDDEN_SYMBOLS

class TestPublisherNode : public b0::Node
{
public:
    TestPublisherNode(std::string topic)
        : Node("publisher"),
          pub_(this, topic)
    {
    }

    void run()
    {
        int i = 0;
        while(!shutdownRequested())
        {
            spinOnce();

            example_msgs::TestMessage msg;
            msg.set_data(i++);
            std::cout << "Sending:" << std::endl << msg.DebugString() << std::endl;
            pub_.publish(msg);

            boost::this_thread::sleep_for(boost::chrono::seconds(1));
        }
    }

private:
    b0::Publisher<example_msgs::TestMessage> pub_;
};

int main(int argc, char **argv)
{
    TestPublisherNode node(argc > 1 ? argv[1] : "A");
    node.init();
    node.run();
    return 0;
}

//! \endcond

