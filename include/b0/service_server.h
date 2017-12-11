#ifndef B0__SERVICE_SERVER_H__INCLUDED
#define B0__SERVICE_SERVER_H__INCLUDED

#include <string>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <b0/socket/socket.h>

namespace b0
{

class Node;

//! \cond HIDDEN_SYMBOLS

class AbstractServiceServer : public socket::Socket
{
public:
    using logger::LogInterface::log;

    AbstractServiceServer(Node *node, std::string service_name, bool managed, bool notify_graph);

    virtual ~AbstractServiceServer();

    void log(LogLevel level, std::string message) const override;

    /*!
     * \brief Perform initialization and optionally send graph notify
     */
    virtual void init() override;

    /*!
     * \brief Perform cleanup and optionally send graph notify
     */
    virtual void cleanup() override;

    std::string getServiceName();

    virtual void bind(std::string address);

protected:
    virtual void bind();
    virtual void unbind();
    virtual void announce();

    //! The ZeroMQ address to bind the service socket on
    std::string bind_addr_;

    const bool notify_graph_;
};

//! \endcond

/*!
 * \brief Service server
 *
 * This class wraps a ZeroMQ REP socket. It will automatically
 * announce the socket name to resolver.
 *
 * If using a callback, when a request is received, it will be handed to the callback, as long as
 * a spin method is called (e.g. Node::spin(), Node::spinOnce() or ServiceServer::spinOnce()).
 *
 * You can directly read requests and write replies from the underlying socket, by using
 * ServiceServer::poll(), ServiceServer::read() and ServiceServer::write().
 *
 * \sa ServiceClient
 */
template<typename TReq, typename TRep>
class ServiceServer : public AbstractServiceServer
{
public:
    /*!
     * \brief Construct a ServiceServer child of a specific Node, using a boost::function as callback
     */
    ServiceServer(Node *node, std::string service_name, boost::function<void(const TReq&, TRep&)> callback = 0, bool managed = true, bool notify_graph = true)
        : AbstractServiceServer(node, service_name, managed, notify_graph),
          callback_(callback)
    {
    }

    /*!
     * \brief Construct a ServiceServer child of a specific Node, using a method (of the Node subclass) as callback
     */
    template<class TNode>
    ServiceServer(TNode *node, std::string service_name, void (TNode::*callbackMethod)(const TReq&, TRep&), bool managed = true, bool notify_graph = true)
        : ServiceServer(node, service_name, boost::bind(callbackMethod, node, _1, _2), managed, notify_graph)
    {
        // delegate constructor. leave empty
    }

    /*!
     * \brief Construct a ServiceServer child of a specific Node, using a method as callback
     */
    template<class T>
    ServiceServer(Node *node, std::string service_name, void (T::*callbackMethod)(const TReq&, TRep&), T *callbackObject, bool managed = true, bool notify_graph = true)
        : ServiceServer(node, service_name, boost::bind(callbackMethod, callbackObject, _1, _2), managed, notify_graph)
    {
        // delegate constructor. leave empty
    }

    /*!
     * \brief Poll and read incoming messages, and dispatch them (called by b0::Node::spinOnce())
     */
    virtual void spinOnce() override
    {
        if(callback_.empty()) return;

        while(poll())
        {
            TReq req;
            TRep rep;
            if(read(req))
            {
                callback_(req, rep);
            }
            write(rep);
        }
    }

protected:
    /*!
     * \brief Callback which will be called when a new message is read from the socket
     */
    boost::function<void(const TReq&, TRep&)> callback_;
};

} // namespace b0

#endif // B0__SERVICE_SERVER_H__INCLUDED
