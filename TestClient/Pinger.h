#pragma once
#include <format>
#include <MessagingMesh.h>
#include "Utils.h"
namespace MM = MessagingMesh;

// Pinger.
class Pinger
{
public:
    // Runs the pinger.
    static void start()
    {
        // We connect to the gateway...
        MM::ConnectionParams connectionParams;
        connectionParams.GatewayHost = "127.0.0.1";
        connectionParams.GatewayPort = 5050;
        connectionParams.Service = "VULCAN";
        MM::Connection connection(connectionParams);

        // We subscribe to pong replies...
        auto totalPingMicroseconds = 0.0;
        auto sampleSize = 10;
        auto count = 0;
        auto pongSubscription = connection.subscribe("PONG", [&](auto& c, auto& /*s*/, auto& /*rs*/, auto m, auto /*t*/)
            {
                // We get the ping time...
                auto us_now = Utils::microsecondsSinceEpoch();
                auto us_ping = m->getSignedInt64("US---");
                auto pingMicroseconds = us_now - us_ping;

                // We log each sample...
                totalPingMicroseconds += pingMicroseconds;
                count++;
                if (count == sampleSize)
                {
                    auto sampleMicroseconds = totalPingMicroseconds / sampleSize;
                    MM::Logger::info(std::format("us={:.2f}", sampleMicroseconds));
                    totalPingMicroseconds = 0.0;
                    count = 0;
                }

                Utils::sleep(100);
                //auto start_us = Utils::microsecondsSinceEpoch();
                //for (; ; )
                //{
                //    auto now_us = Utils::microsecondsSinceEpoch();
                //    auto elapsed_us = now_us - start_us;
                //    if (elapsed_us > 10000.0)
                //    {
                //        break;
                //    }
                //}

                // We send the next ping...
                auto nextPing = MM::Message::create();
                nextPing->addSignedInt64("US---", Utils::microsecondsSinceEpoch());
                c.sendMessage(nextPing, "PING");
            });

        // We send an initial ping to kick things off...
        auto intialPing = MM::Message::create();
        intialPing->addSignedInt64("US---", Utils::microsecondsSinceEpoch());
        connection.sendMessage(intialPing, "PING");

        Utils::processMessages(connection);
    }
};

