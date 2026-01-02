import * as signalR from '@microsoft/signalr';
import { ServiceOverview } from '../types/stats';

export type ServiceOverviewsUpdateCallback = (data: ServiceOverview[]) => void;

class SignalRService {
  private connection: signalR.HubConnection | null = null;
  private callbacks: ServiceOverviewsUpdateCallback[] = [];

  async start(): Promise<void> {
    if (this.connection) {
      return;
    }

    // Build the connection to the StatsHub
    this.connection = new signalR.HubConnectionBuilder()
      .withUrl('/hubs/stats')
      .withAutomaticReconnect()
      .configureLogging(signalR.LogLevel.Information)
      .build();

    // Register the handler for service overview updates
    this.connection.on('ServiceOverviewsUpdate', (data: ServiceOverview[]) => {
      this.callbacks.forEach(callback => callback(data));
    });

    // Handle reconnection
    this.connection.onreconnected(() => {
      console.log('SignalR reconnected');
    });

    this.connection.onreconnecting(() => {
      console.log('SignalR reconnecting...');
    });

    this.connection.onclose(() => {
      console.log('SignalR connection closed');
    });

    try {
      await this.connection.start();
      console.log('SignalR connected');
    } catch (err) {
      console.error('SignalR connection error:', err);
      throw err;
    }
  }

  async stop(): Promise<void> {
    if (this.connection) {
      await this.connection.stop();
      this.connection = null;
    }
  }

  onServiceOverviewsUpdate(callback: ServiceOverviewsUpdateCallback): () => void {
    this.callbacks.push(callback);
    
    // Return unsubscribe function
    return () => {
      const index = this.callbacks.indexOf(callback);
      if (index > -1) {
        this.callbacks.splice(index, 1);
      }
    };
  }

  getConnectionState(): signalR.HubConnectionState | null {
    return this.connection?.state ?? null;
  }
}

// Export a singleton instance
export const signalRService = new SignalRService();
