import { useState, useEffect } from 'react';
import { Network } from 'lucide-react';
import { ServiceCard } from '../components/ServiceCard';
import { ConnectionStatus } from '../components/ConnectionStatus';
import { signalRService } from '../services/signalRService';
import { ServiceOverview } from '../types/stats';
import { HubConnectionState } from '@microsoft/signalr';

export function Dashboard() {
  const [services, setServices] = useState<ServiceOverview[]>([]);
  const [connectionState, setConnectionState] = useState<HubConnectionState | null>(null);
  const [lastUpdate, setLastUpdate] = useState<Date | null>(null);

  useEffect(() => {
    // Start the SignalR connection
    signalRService.start().catch(err => {
      console.error('Failed to start SignalR connection:', err);
    });

    // Subscribe to service overview updates
    const unsubscribe = signalRService.onServiceOverviewsUpdate((data) => {
      setServices(data);
      setLastUpdate(new Date());
    });

    // Poll connection state
    const stateInterval = setInterval(() => {
      setConnectionState(signalRService.getConnectionState());
    }, 500);

    // Cleanup on unmount
    return () => {
      unsubscribe();
      clearInterval(stateInterval);
      signalRService.stop();
    };
  }, []);

  const handleServiceClick = (serviceName: string) => {
    console.log('Clicked service:', serviceName);
    // TODO: Navigate to service detail page
    // navigate(`/service/${serviceName}`);
  };

  return (
    <div className="dashboard">
      <header className="dashboard-header">
        <div className="header-content">
          <div className="dashboard-title">
            <Network size={32} className="title-icon" />
            <h1>Messaging Mesh</h1>
          </div>
          <div className="header-info">
            <ConnectionStatus state={connectionState} />
            {lastUpdate && (
              <span className="last-update">
                Last update: {lastUpdate.toLocaleTimeString()}
              </span>
            )}
          </div>
        </div>
      </header>

      <main className="dashboard-main">
        <div className="services-header">
          <h2>Active Services</h2>
          <div className="service-count">
            {services.length} {services.length === 1 ? 'service' : 'services'}
          </div>
        </div>

        {services.length === 0 ? (
          <div className="no-services">
            <p>No active services</p>
            <p className="no-services-hint">
              Waiting for statistics from gateways...
            </p>
          </div>
        ) : (
          <div className="services-grid">
            {services.map((service) => (
              <ServiceCard
                key={service.serviceName}
                service={service}
                onClick={() => handleServiceClick(service.serviceName)}
              />
            ))}
          </div>
        )}
      </main>
    </div>
  );
}
