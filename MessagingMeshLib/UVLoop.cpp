#include "UVLoop.h"
#include "Logger.h"
#include "MMUtils.h"
#include "UVUtils.h"
using namespace MessagingMesh;

// Constructor.
UVLoop::UVLoop(const std::string& name) :
    m_name(name)
{
    Logger::info("Creating UV loop: " + m_name);

    // We create the thread...
    uv_thread_create(
        &m_threadHandle, 
        [](void* args)
        {
            auto self = (UVLoop*)args;
            self->threadEntryPoint();
        },
        this);
}

// Destructor.
UVLoop::~UVLoop()
{
    // We marshall an event to the loop to tell it to stop...
    Logger::info("Signalling UV loop to stop: " + m_name);
    marshallEvent(
        [](uv_loop_t* pLoop)
        {
            uv_stop(pLoop);
        });

    // We wait for the thread to end...
    uv_thread_join(&m_threadHandle);
    Logger::info("UV loop stopped: " + m_name);
}

// Thread entry point.
void UVLoop::threadEntryPoint()
{
    try
    {
        // We set the thread's name...
        UVUtils::setThreadName(m_name);

        // We create the uv loop and tell it that it is associated with 
        // this UVLoopThread object...
        m_loop = std::make_unique<uv_loop_t>();
        m_loop->data = this;
        uv_loop_init(m_loop.get());

        // We listen for marshalled events...
        m_marshalledEventsSignal = std::make_unique<uv_async_t>();
        uv_async_init(
            m_loop.get(),
            m_marshalledEventsSignal.get(),
            [](uv_async_t* pHandle)
            {
                auto self = (UVLoop*)pHandle->loop->data;
                self->processMarshalledEvents();
            });

        // We signal the event in case there are already marshalled events...
        uv_async_send(m_marshalledEventsSignal.get());

        // We run the loop...
        Logger::info("Running UV event loop for: " + m_name);
        uv_run(m_loop.get(), UV_RUN_DEFAULT);
    }
    catch (const std::exception& ex)
    {
        Logger::error(MMUtils::format("%s: %s", __func__, ex.what()));
    }
}

// Marshalls an event to the UV loop we are managing. This event (function)
// will be called from within the event loop.
void UVLoop::marshallEvent(MarshalledEvent marshalledEvent)
{
    // We add the event to the collection of marshalled events.
    m_marshalledEvents.add(marshalledEvent);

    // We signal to the event loop that there is a new event.
    // Note: We check that the loop and signal have been set up. If not, we
    //       cannot signal, but the event has still been added to the queue.
    if (m_marshalledEventsSignal)
    {
        uv_async_send(m_marshalledEventsSignal.get());
    }
}

// Marshalls an event to the UV loop we are managing. 
// Only one event for the given key will be marshalled until events
// are processed in the UV loop thread.
void UVLoop::marshallUniqueEvent(const UniqueEventKey& key, MarshalledEvent marshalledEvent)
{
    // We add the event to the collection of marshalled events.
    auto itemAdded = m_marshalledEvents.addUnique(key, marshalledEvent);

    // We signal to the event loop that there is a new event.
    // Note: We check that the loop and signal have been set up. If not, we
    //       cannot signal, but the event has still been added to the queue.
    if (itemAdded && m_marshalledEventsSignal)
    {
        uv_async_send(m_marshalledEventsSignal.get());
    }
}

// Processes marshalled events.
void UVLoop::processMarshalledEvents()
{
    try
    {
        // We get the marshalled events and process them...
        auto marshalledEvents = m_marshalledEvents.getItems();
        for (auto marshalledEvent : *marshalledEvents)
        {
            marshalledEvent(m_loop.get());
        }
    }
    catch (const std::exception& ex)
    {
        Logger::error(MMUtils::format("%s: %s", __func__, ex.what()));
    }
}
