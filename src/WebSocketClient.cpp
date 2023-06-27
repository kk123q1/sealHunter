#include "WebSocketClient.h"
#include "ltclientshell.h"

WebSocketClient::WebSocketClient(CLTClientShell* pClientShell, const std::string& url)
    : m_pClientShell(pClientShell), m_url(url)
{
    m_client.set_access_channels(websocketpp::log::alevel::none);
    m_client.set_error_channels(websocketpp::log::elevel::all);
    m_client.init_asio();
    m_client.set_message_handler(websocketpp::lib::bind(&WebSocketClient::OnMessage, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
}

WebSocketClient::~WebSocketClient()
{
    Disconnect();
}

void WebSocketClient::Connect()
{
    m_connection = m_client.get_connection(m_url, m_ec);
    if (m_ec) {
        std::cerr << "Error creating WebSocket connection: " << m_ec.message() << std::endl;
        return;
    }
    m_client.connect(m_connection);
    m_thread = std::thread(&client::run, &m_client);
}

void WebSocketClient::Disconnect()
{
    m_client.stop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void WebSocketClient::OnMessage(websocketpp::connection_hdl hdl, client::message_ptr msg)
{
    // Here you handle the message, for example by creating a monster
    // In this example, we just print the message to the console
    std::string payload = msg->get_payload();
    std::cout << "Received message: " << payload << std::endl;

    // If payload contains information about a monster, create it
    // You need to parse the payload to extract this information
    if (payload.find("\"giftId\":463") != std::string::npos) {
        m_pClientShell->CreateMonster();
    }
}
