#ifndef B0__SERVICE_SERVER_H__INCLUDED
#define B0__SERVICE_SERVER_H__INCLUDED

#include <string>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <b0/socket.h>

namespace b0
{

class Node;

/*!
 * \brief The service server class
 *
 * This class wraps a REP socket. It will automatically
 * announce the socket name to resolver.
 *
 * \sa b0::ServiceClient, b0::ServiceServer
 */
class ServiceServer : public Socket
{
public:
    using logger::LogInterface::log;

    //! \cond HIDDEN_SYMBOLS

    // needed to disambiguate the overloaded constructors
    struct CallbackWithoutType {};
    struct CallbackWithType {};
    struct CallbackRawParts {};

    //! \endcond

    /*!
     * \brief Construct an ServiceServer child of the specified Node, optionally using a boost::function as a callback
     */
    ServiceServer(Node *node, std::string service_name, boost::function<void(const std::string&, std::string&)> callback = 0, bool managed = true, bool notify_graph = true)
        : ServiceServer(node, service_name, CallbackWithoutType(), callback, managed, notify_graph)
    {}

    /*!
     * \brief Construct an ServiceServer child of the specified Node, optionally using a boost::function as a callback
     */
    ServiceServer(Node *node, std::string service_name, const CallbackWithoutType &_, boost::function<void(const std::string&, std::string&)> callback, bool managed = true, bool notify_graph = true);

    /*!
     * \brief Construct an ServiceServer child of the specified Node, optionally using a boost::function as a callback (including type argument)
     */
    ServiceServer(Node *node, std::string service_name, const CallbackWithType &_, boost::function<void(const std::string&, const std::string&, std::string&, std::string&)> callback, bool managed = true, bool notify_graph = true);

    /*!
     * \brief Construct an ServiceServer child of the specified Node, optionally using a boost::function as a callback (raw message parts)
     */
    ServiceServer(Node *node, std::string service_name, const CallbackRawParts &_, boost::function<void(const std::vector<b0::message::MessagePart>&, std::vector<b0::message::MessagePart>&)> callback, bool managed = true, bool notify_graph = true);

    /*!
     * \brief ServiceServer destructor
     */
    virtual ~ServiceServer();

    /*!
     * \brief Log a message using node's logger, prepending this service server informations
     */
    void log(LogLevel level, std::string message) const override;

    /*!
     * \brief Perform initialization and optionally send graph notify
     */
    virtual void init() override;

    /*!
     * \brief Perform cleanup and optionally send graph notify
     */
    virtual void cleanup() override;

    /*!
     * \brief Poll and read incoming messages, and dispatch them (called by b0::Node::spinOnce())
     */
    virtual void spinOnce() override;

    /*!
     * \brief Return the name of this server's service
     */
    std::string getServiceName();

    /*!
     * \brief Bind to an additional address
     */
    virtual void bind(std::string address);

protected:
    /*!
     * \brief Bind socket to the address
     */
    virtual void bind();

    /*!
     * \brief Unbind socket from the address
     */
    virtual void unbind();

    /*!
     * \brief Announce service to resolver
     */
    virtual void announce();

    //! The ZeroMQ address to bind the service socket on
    std::string bind_addr_;

    //! If false this socket will not send announcement to resolv (i.e. it will be "invisible")
    const bool notify_graph_;

    /*!
     * \brief Callback which will be called when a new message is read from the socket (raw)
     */
    boost::function<void(const std::string&, std::string&)> callback_;

    /*!
     * \brief Callback which will be called when a new message is read from the socket (raw with type)
     */
    boost::function<void(const std::string&, const std::string&, std::string&, std::string&)> callback_with_type_;

    /*!
     * \brief Callback which will be called when a new message is read from the socket (raw multipart)
     */
    boost::function<void(const std::vector<b0::message::MessagePart>&, std::vector<b0::message::MessagePart>&)> callback_multipart_;
};

} // namespace b0

#endif // B0__SERVICE_SERVER_H__INCLUDED
