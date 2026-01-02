import { HubConnectionState } from '@microsoft/signalr';

interface ConnectionStatusProps {
  state: HubConnectionState | null;
}

export function ConnectionStatus({ state }: ConnectionStatusProps) {
  const getStatusInfo = () => {
    switch (state) {
      case HubConnectionState.Connected:
        return { text: 'Connected', color: '#059669', icon: '●' };
      case HubConnectionState.Connecting:
        return { text: 'Connecting...', color: '#d97706', icon: '◐' };
      case HubConnectionState.Reconnecting:
        return { text: 'Reconnecting...', color: '#d97706', icon: '◑' };
      case HubConnectionState.Disconnected:
        return { text: 'Disconnected', color: '#dc2626', icon: '○' };
      default:
        return { text: 'Unknown', color: '#6b7280', icon: '?' };
    }
  };

  const status = getStatusInfo();

  return (
    <div className="connection-status">
      <span className="status-icon" style={{ color: status.color }}>
        {status.icon}
      </span>
      <span className="status-text">{status.text}</span>
    </div>
  );
}
