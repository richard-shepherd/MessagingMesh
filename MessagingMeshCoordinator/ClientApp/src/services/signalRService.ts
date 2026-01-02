import * as signalR from '@microsoft/signalr';
import { ServiceOverview, ServiceDetails } from '../types/stats';

export type ServiceOverviewsUpdateCallback = (data: ServiceOverview[]) => void;
export type ServiceDetailsUpdateCallback = (data: ServiceDetails) => void;

class SignalRService {
  private connection: signalR.HubConnection | null = null;
  private overviewCallbacks: ServiceOverviewsUpdateCallback[] = [];
  private detailsCallbacks: Map<string, ServiceDetailsUpdateCallback[]> = new Map();

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
      this.overviewCallbacks.forEach(callback => callback(data));
    });

    // Register the handler for service detail updates
    this.connection.on('ServiceDetailsUpdate', (data: ServiceDetails) => {
      const callbacks = this.detailsCallbacks.get(data.serviceName);
      if (callbacks) {
        callbacks.forEach(callback => callback(data));
      }
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
    this.overviewCallbacks.push(callback);
    
    // Return unsubscribe function
    return () => {
      const index = this.overviewCallbacks.indexOf(callback);
      if (index > -1) {
        this.overviewCallbacks.splice(index, 1);
      }
    };
  }

  async subscribeToServiceDetails(serviceName: string): Promise<void> {
    if (!this.connection) {
      throw new Error('SignalR connection not established');
    }
    await this.connection.invoke('SubscribeToServiceDetails', serviceName);
  }

  async unsubscribeFromServiceDetails(serviceName: string): Promise<void> {
    if (!this.connection) {
      return;
    }
    await this.connection.invoke('UnsubscribeFromServiceDetails', serviceName);
  }

  onServiceDetailsUpdate(serviceName: string, callback: ServiceDetailsUpdateCallback): () => void {
    if (!this.detailsCallbacks.has(serviceName)) {
      this.detailsCallbacks.set(serviceName, []);
    }
    this.detailsCallbacks.get(serviceName)!.push(callback);
    
    // Return unsubscribe function
    return () => {
      const callbacks = this.detailsCallbacks.get(serviceName);
      if (callbacks) {
        const index = callbacks.indexOf(callback);
        if (index > -1) {
          callbacks.splice(index, 1);
        }
        // Clean up empty arrays
        if (callbacks.length === 0) {
          this.detailsCallbacks.delete(serviceName);
        }
      }
    };
  }

  getConnectionState(): signalR.HubConnectionState | null {
    return this.connection?.state ?? null;
  }
}

// Export a singleton instance
export const signalRService = new SignalRService();
