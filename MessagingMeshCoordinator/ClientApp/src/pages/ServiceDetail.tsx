import { useState, useEffect } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { ArrowLeft, Play, Pause, PieChart as PieChartIcon } from 'lucide-react';
import { signalRService } from '../services/signalRService';
import { ServiceDetails } from '../types/stats';
import { PieChart } from '../components/PieChart';
import { ConnectionStatus } from '../components/ConnectionStatus';
import { HubConnectionState } from '@microsoft/signalr';

export function ServiceDetail() {
  const { serviceName } = useParams<{ serviceName: string }>();
  const navigate = useNavigate();
  const [serviceDetails, setServiceDetails] = useState<ServiceDetails | null>(null);
  const [isPaused, setIsPaused] = useState(false);
  const [connectionState, setConnectionState] = useState<HubConnectionState | null>(null);
  const [lastUpdate, setLastUpdate] = useState<Date | null>(null);

  useEffect(() => {
    if (!serviceName) {
      navigate('/');
      return;
    }

    let isSubscribed = false;

    const setupSubscription = async () => {
      try {
        // Ensure SignalR is connected
        await signalRService.start();

        // Subscribe to service details
        await signalRService.subscribeToServiceDetails(serviceName);
        isSubscribed = true;

        // Listen for updates
        const unsubscribe = signalRService.onServiceDetailsUpdate(serviceName, (data) => {
          if (!isPaused) {
            setServiceDetails(data);
            setLastUpdate(new Date());
          }
        });

        // Poll connection state
        const stateInterval = setInterval(() => {
          setConnectionState(signalRService.getConnectionState());
        }, 500);

        // Cleanup
        return () => {
          unsubscribe();
          clearInterval(stateInterval);
          if (isSubscribed) {
            signalRService.unsubscribeFromServiceDetails(serviceName);
          }
        };
      } catch (err) {
        console.error('Failed to subscribe to service details:', err);
      }
    };

    const cleanup = setupSubscription();

    return () => {
      cleanup.then(fn => fn && fn());
    };
  }, [serviceName, navigate, isPaused]);

  const handleBackClick = () => {
    navigate('/');
  };

  const handleTogglePause = () => {
    setIsPaused(!isPaused);
  };

  if (!serviceName) {
    return null;
  }

  return (
    <div className="service-detail">
      <header className="detail-header">
        <div className="header-content">
          <div className="header-left">
            <button className="back-button" onClick={handleBackClick}>
              <ArrowLeft size={20} />
              <span>Back to Services</span>
            </button>
            <div className="service-title-section">
              <PieChartIcon size={28} className="detail-icon" />
              <h1 className="service-title">{serviceName}</h1>
            </div>
          </div>
          <div className="header-right">
            <button 
              className={`pause-button ${isPaused ? 'paused' : ''}`}
              onClick={handleTogglePause}
              title={isPaused ? 'Resume updates' : 'Pause updates'}
            >
              {isPaused ? <Play size={18} /> : <Pause size={18} />}
              <span>{isPaused ? 'Resume' : 'Pause'}</span>
            </button>
            <ConnectionStatus state={connectionState} />
            {lastUpdate && (
              <span className="last-update">
                Last update: {lastUpdate.toLocaleTimeString()}
              </span>
            )}
          </div>
        </div>
      </header>

      <main className="detail-main">
        {!serviceDetails ? (
          <div className="loading-state">
            <div className="loading-spinner"></div>
            <p>Loading service details...</p>
          </div>
        ) : (
          <div className="charts-grid">
            <div className="chart-card">
              <div className="chart-header">
                <h2>Top Subjects by Messages/sec</h2>
                <div className="chart-subtitle">
                  Real-time message throughput by subject
                </div>
              </div>
              <div className="chart-container">
                <PieChart 
                  data={serviceDetails.topSubjects_MessagesPerSecond}
                  dataKey="messagesPerSecond"
                  colorScheme="blue"
                />
              </div>
            </div>

            <div className="chart-card">
              <div className="chart-header">
                <h2>Top Subjects by Megabits/sec</h2>
                <div className="chart-subtitle">
                  Real-time bandwidth usage by subject
                </div>
              </div>
              <div className="chart-container">
                <PieChart 
                  data={serviceDetails.topSubjects_MegaBitsPerSecond}
                  dataKey="megaBitsPerSecond"
                  colorScheme="green"
                />
              </div>
            </div>
          </div>
        )}

        {isPaused && serviceDetails && (
          <div className="paused-banner">
            Updates paused - Click Resume to continue receiving live data
          </div>
        )}
      </main>
    </div>
  );
}
